#ifndef AIRBALL_SETTINGS_STORE_H
#define AIRBALL_SETTINGS_STORE_H

#include <vector>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <boost/format.hpp>

namespace airball {

class Parameter {
public:
  Parameter(const std::string& json_key, const std::string& display_name)
      : json_key_(json_key), display_name_(display_name) {}

  void load(const rapidjson::Document &doc) {
    if (doc.HasMember(json_key().c_str())) {
      loadImpl(doc);
    }
  }

  void save(rapidjson::Document &doc) const {
    saveImpl(doc);
  }

  [[nodiscard]] virtual std::string display_value() const = 0;

  [[nodiscard]] std::string display_name() const { return display_name_; }

  virtual void increment() = 0;
  virtual void decrement() = 0;

protected:
  [[nodiscard]] std::string json_key() const { return json_key_; }

  virtual void loadImpl(const rapidjson::Document &doc) = 0;
  virtual void saveImpl(rapidjson::Document &doc) const = 0;

private:
  const std::string json_key_;
  const std::string display_name_;
};

template<typename T>
class TypedParameter : public Parameter {
public:
  TypedParameter(const std::string& json_key,
                 const std::string& display_name,
                 const T& initial)
      : Parameter(json_key, display_name),
        value_(initial) {}

  [[nodiscard]] T get() const { return value_; }

  [[nodiscard]] std::string display_value() const override;

protected:
  void set(const T& value) { value_ = value; }

  void loadImpl(const rapidjson::Document &doc) override;
  void saveImpl(rapidjson::Document &doc) const override;

private:
  T value_;
};

template<>
void TypedParameter<int>::loadImpl(const rapidjson::Document &doc) {
  set(doc[json_key().c_str()].GetInt());
}

template<>
void TypedParameter<int>::saveImpl(rapidjson::Document &doc) const {
  doc.AddMember(
      rapidjson::Value(json_key().c_str(), doc.GetAllocator()).Move(),
      rapidjson::Value(value_).Move(),
      doc.GetAllocator());
}

template<>
std::string TypedParameter<int>::display_value() const {
  return str(boost::format("%d") % get());
}

template<>
void TypedParameter<double>::loadImpl(const rapidjson::Document &doc) {
  value_ = doc[json_key().c_str()].GetDouble();
}

template<>
void TypedParameter<double>::saveImpl(rapidjson::Document &doc) const {
  doc.AddMember(
      rapidjson::Value(json_key().c_str(), doc.GetAllocator()).Move(),
      rapidjson::Value(value_).Move(),
      doc.GetAllocator());
}

template<>
std::string TypedParameter<double>::display_value() const {
  return str(boost::format("%f") % get());
}

template<>
void TypedParameter<bool>::loadImpl(const rapidjson::Document &doc) {
  value_ = doc[json_key().c_str()].GetBool();
}

template<>
void TypedParameter<bool>::saveImpl(rapidjson::Document &doc) const {
  doc.AddMember(
      rapidjson::Value(json_key().c_str(), doc.GetAllocator()).Move(),
      rapidjson::Value(value_).Move(),
      doc.GetAllocator());
}

template<>
std::string TypedParameter<bool>::display_value() const {
  return get() ? "yes" : "no";
}

class StringSelectionParameter : public Parameter {
public:
  StringSelectionParameter(const std::string& json_key,
                           const std::string& display_name,
                           const std::vector<std::string>& options,
                           const size_t initial_index)
      : Parameter(json_key, display_name),
        current_index_(initial_index),
        options_(options) {}

  [[nodiscard]] std::string get() const {
    return options_[current_index_];
  }

  [[nodiscard]] std::string display_value() const override {
    return get();
  };

  void increment() override {
    current_index_ = std::min(current_index_ + 1, options_.size() - 1);
  }

  void decrement() override {
    // Caution about using std::max here since decrementing an unsigned
    // will yield a very large positive number.
    if (current_index_ > 0) {
      current_index_--;
    }
  }

protected:
  void loadImpl(const rapidjson::Document &doc) override {
    std::string string_value = doc[json_key().c_str()].GetString();
    auto i = std::find(options_.begin(), options_.end(), string_value);
    if (i != options_.end()) {
      current_index_ = i - options_.begin();
    }
  }

  void saveImpl(rapidjson::Document &doc) const override {
    doc.AddMember(
        rapidjson::Value(json_key().c_str(), doc.GetAllocator()).Move(),
        rapidjson::Value().SetString(options_[current_index_].c_str(), doc.GetAllocator()).Move(),
        doc.GetAllocator());
  }

private:
  size_t current_index_;
  const std::vector<std::string> options_;
};

class BoolParameter : public TypedParameter<bool> {
public:
  BoolParameter(const std::string& json_key,
                const std::string& display_name,
                const bool& initial)
      : TypedParameter<bool>(json_key, display_name, initial) {}

  void increment() override {
    if (!get()) {
      set(true);
    }
  }

  void decrement() override {
    if (get()) {
      set(false);
    }
  }
};

template<typename T>
class NumericParameter : public TypedParameter<T> {
public:
  NumericParameter(const std::string &json_key,
                   const std::string& display_name,
                   const T &initial,
                   const T &min,
                   const T &max,
                   const T &increment,
                   const std::string& format)
      : TypedParameter<T>(json_key, display_name, initial),
        min_(min),
        max_(max),
        increment_(increment),
        format_(format) {}

  void increment() override {
    TypedParameter<T>::set(clamp(this->get() + increment_));
  }

  void decrement() override {
    TypedParameter<T>::set(clamp(this->get() - increment_));
  }

  [[nodiscard]] std::string display_value() const override {
    return str(boost::format(format_) % this->get());
  }

private:
  T clamp(T value) {
    T v = (value / increment_) * increment_;
    v = std::max(min_, v);
    v = std::min(max_, v);
    return v;
  }

  const T min_;
  const T max_;
  const T increment_;
  const std::string format_;
};

class SpeedParameter : public NumericParameter<double> {
public:
  SpeedParameter(const StringSelectionParameter *speed_units,
                 const std::string &json_key,
                 const std::string &display_name,
                 const double &initial,
                 const double &min,
                 const double &max,
                 const double &increment,
                 const std::string& format)
      : NumericParameter<double>(json_key, display_name, initial, min, max, increment, format),
        speed_units_(speed_units) {}

  [[nodiscard]] std::string display_value() const override {
    return str(boost::format("%s %s") %
               NumericParameter < double > ::display_value() %
                                           speed_units_->get());
  }

private:
  const StringSelectionParameter* speed_units_;
};

class AngleParameter : public NumericParameter<double> {
public:
  AngleParameter(const std::string &json_key,
                 const std::string &display_name,
                 const double &initial,
                 const double &min,
                 const double &max,
                 const double &increment,
                 const std::string& format)
      : NumericParameter<double>(json_key, display_name, initial, min, max, increment, format) {}

  [[nodiscard]] std::string display_value() const override {
    return str(boost::format("%s °") %
               NumericParameter < double > ::display_value());
  }
};

class SettingsStore {
public:
  StringSelectionParameter SPEED_UNITS {
    "speed_units",
    "SPD",
    {"knots", "mph"},
    0,
  };
  SpeedParameter IAS_FULL_SCALE {
    &SPEED_UNITS,
    "ias_full_scale",
    "V_FS",
    100, 0, 300, 1,
    "%3.0f",
  };
  SpeedParameter V_R {
    &SPEED_UNITS,
    "v_r",
    "V_R",
    50, 0, 300, 1,
    "%3.0f",
  };
  SpeedParameter V_FE {
    &SPEED_UNITS,
    "v_fe",
    "V_FE",
    75, 0, 300, 1,
    "%3.0f",
  };
  SpeedParameter V_NO {
    &SPEED_UNITS,
    "v_no",
    "V_NO",
    100, 0, 300, 1,
    "%3.0f",
  };
  SpeedParameter V_NE {
    &SPEED_UNITS,
    "v_ne",
    "V_NE",
    100, 0, 300, 1,
    "%3.0f",
  };
  AngleParameter ALPHA_STALL {
    "alpha_stall",
    "α_CRIT",
    15.0, -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_STALL_WARNING {
    "alpha_stall_warning",
    "α_CRIT_W",
    14.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_MIN {
    "alpha_min",
    "α_MIN",
    -10.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_MAX {
    "alpha_max",
    "α_MAX",
    20.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_X {
    "alpha_x",
    "α_X",
    12.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_Y {
    "alpha_y",
    "α_Y",
    10.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_REF {
    "alpha_ref",
    "α_REF",
    14.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter BETA_FULL_SCALE {
    "beta_full_scale",
    "β_FS",
    20.0,
    0, 30.0, 5,
    "%4.1f",
  };
  AngleParameter BETA_BIAS {
    "beta_bias",
    "β BIAS",
    0.0,
    0, 30.0, 0.1,
    "%4.1f",
  };
  NumericParameter<double> BARO_SETTING {
    "baro_setting",
    "BARO",
    29.92,
    25, 35, 0.01,
    "%5.2f",
  };
  NumericParameter<double> BALL_TIME_CONSTANT {
      "ball_time_constant",
      "BALL T",
      0.5,
      0, 1.0, 0.1,
      "%4.2f",
  };
  NumericParameter<double> VSI_TIME_CONSTANT {
    "vsi_time_constant",
    "VSI T",
    1.0,
    0.1, 5.0, 0.1,
    "%3.1f",
  };
  NumericParameter<int> SCREEN_WIDTH {
    "screen_width",
    "DO_NOT_DISPLAY",
    272,
    272, 272, 0,
    "%d",
  };
  NumericParameter<int> SCREEN_HEIGHT {
    "screen_height",
    "DO_NOT_DISPLAY",
    480,
    480, 480, 0,
    "%d",
  };
  BoolParameter SHOW_ALTIMETER {
    "show_altimeter",
    "ALT?",
    true,
  };
  BoolParameter SHOW_LINK_STATUS {
    "show_link_status",
    "LINK?",
    true,
  };
  BoolParameter SHOW_PROBE_BATTERY_STATUS {
    "show_probe_battery_status",
    "BAT?",
    true,
  };
  BoolParameter DECLUTTER {
    "declutter",
    "DCLTR?",
    false,
  };
  StringSelectionParameter SOUND_SCHEME {
      "sound_scheme",
      "SND",
      {"stallfence", "flyonspeed"},
      0,
  };
  NumericParameter<double> AUDIO_VOLUME {
    "audio_volume",
    "VOL",
    1.0,
    0, 1.0, 0.05,
    "%4.2f",
  };
  BoolParameter ROTATE_SCREEN {
      "rotate_screen",
      "DO_NOT_DISPLAY",
      false,
  };
  NumericParameter<double> SCREEN_BRIGHTNESS {
    "screen_brightness",
    "BRT",
    1.0,
    0, 1.0, 0.05,
    "%4.2f",
  };
  BoolParameter SHOW_NUMERIC_AIRSPEED {
      "show_numeric_airspeed",
      "SPD?",
      true,
  };

  const std::vector<Parameter *> ALL_PARAMS = {
      (Parameter*) &IAS_FULL_SCALE,
      (Parameter*) &V_R,
      (Parameter*) &V_FE,
      (Parameter*) &V_NO,
      (Parameter*) &V_NE,
      (Parameter*) &ALPHA_STALL,
      (Parameter*) &ALPHA_STALL_WARNING,
      (Parameter*) &ALPHA_MIN,
      (Parameter*) &ALPHA_MAX,
      (Parameter*) &ALPHA_X,
      (Parameter*) &ALPHA_Y,
      (Parameter*) &ALPHA_REF,
      (Parameter*) &BETA_FULL_SCALE,
      (Parameter*) &BETA_BIAS,
      (Parameter*) &BARO_SETTING,
      (Parameter*) &BALL_TIME_CONSTANT,
      (Parameter*) &VSI_TIME_CONSTANT,
      (Parameter*) &SCREEN_WIDTH,
      (Parameter*) &SCREEN_HEIGHT,
      (Parameter*) &SHOW_ALTIMETER,
      (Parameter*) &SHOW_LINK_STATUS,
      (Parameter*) &SHOW_PROBE_BATTERY_STATUS,
      (Parameter*) &DECLUTTER,
      (Parameter*) &SOUND_SCHEME,
      (Parameter*) &AUDIO_VOLUME,
      (Parameter*) &SPEED_UNITS,
      (Parameter*) &ROTATE_SCREEN,
      (Parameter*) &SCREEN_BRIGHTNESS,
      (Parameter*) &SHOW_NUMERIC_AIRSPEED,
  };
};

} // namespace airball

#endif // AIRBALL_SETTINGS_STORE_H
