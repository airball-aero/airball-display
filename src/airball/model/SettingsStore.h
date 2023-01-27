#ifndef AIRBALL_SETTINGS_STORE_H
#define AIRBALL_SETTINGS_STORE_H

#include <vector>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <boost/format.hpp>

namespace airball {

class Parameter {
public:
  Parameter(const std::string& json_key, const std::string& display_name, ISettings::Adjustment adjustment)
      : json_key_(json_key), display_name_(display_name), adjustment_(adjustment) {}

  void load(const rapidjson::Document &doc) {
    if (doc.HasMember(json_key().c_str())) {
      loadImpl(doc);
    }
  }

  void save(rapidjson::Document &doc) const {
    saveImpl(doc);
  }

  virtual std::string display_value() const = 0;

  std::string display_name() const { return display_name_; }

  ISettings::Adjustment adjustment() { return adjustment_; }

  virtual void hidIncrement() = 0;
  virtual void hidDecrement() = 0;

protected:
  std::string json_key() const { return json_key_; }

  virtual void loadImpl(const rapidjson::Document &doc) = 0;
  virtual void saveImpl(rapidjson::Document &doc) const = 0;

private:
  const ISettings::Adjustment adjustment_;
  const std::string json_key_;
  const std::string display_name_;
};

template<typename T>
class TypedParameter : public Parameter {
public:
  TypedParameter(const std::string& json_key,
                 const std::string& display_name,
                 ISettings::Adjustment adjustment,
                 const T& initial)
      : Parameter(json_key, display_name, adjustment),
        value_(initial) {}

  T get() const { return value_; }

  std::string display_value() const override;

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
  return get() ? "true" : "false";
}

class StringSelectionParameter : public Parameter {
public:
  StringSelectionParameter(const std::string& json_key,
                           const std::string& display_name,
                           ISettings::Adjustment adjustment,
                           const std::vector<std::string>& options,
                           const size_t initial_index)
      : Parameter(json_key, display_name, adjustment),
        current_index_(initial_index),
        options_(options) {}

  std::string get() const {
    return options_[current_index_];
  }

  std::string display_value() const override {
    return get();
  };

  void hidIncrement() override {
    current_index_ = (current_index_ + 1) % options_.size();
  }

  void hidDecrement()  override {
    current_index_ = (current_index_ - 1) % options_.size();
  }

protected:
  void loadImpl(const rapidjson::Document &doc) {
    std::string string_value = doc[json_key().c_str()].GetString();
    auto i = std::find(options_.begin(), options_.end(), string_value);
    if (i != options_.end()) {
      current_index_ = i - options_.begin();
    }
  }

  void saveImpl(rapidjson::Document &doc) const {
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
                ISettings::Adjustment adjustment,
                const bool& initial)
      : TypedParameter<bool>(json_key, display_name, adjustment, initial) {}

  void hidIncrement() override {
    set(!get());
  }

  void hidDecrement()  override {
    set(!get());
  }
};

template<typename T>
class NumericParameter : public TypedParameter<T> {
public:
  NumericParameter(const std::string &json_key,
                   const std::string& display_name,
                   ISettings::Adjustment adjustment,
                   const T &initial,
                   const T &min,
                   const T &max,
                   const T &increment)
      : TypedParameter<T>(json_key, display_name, adjustment, initial),
        min_(min),
        max_(max),
        increment_(increment) {}

  void hidIncrement() override {
    TypedParameter<T>::set(clamp(this->get() + increment_));
  }

  void hidDecrement() override {
    TypedParameter<T>::set(clamp(this->get() - increment_));
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
};

class SettingsStore {
public:
  NumericParameter<double> IAS_FULL_SCALE {
    "ias_full_scale",
    "V_FS",
    ISettings::ADJUSTMENT_IAS_FULL_SCAlE,
    100, 0, 300, 1,
  };
  NumericParameter<double> V_R {
    "v_r",
    "V_R",
    ISettings::ADJUSTMENT_V_R,
    50, 0, 300, 1,
  };
  NumericParameter<double> V_FE {
    "v_fe",
    "V_FE",
    ISettings::ADJUSTMENT_V_FE,
    75, 0, 300, 1,
  };
  NumericParameter<double> V_NO {
    "v_no",
    "V_NO",
    ISettings::ADJUSTMENT_V_NO,
    100, 0, 300, 1,
  };
  NumericParameter<double> V_NE {
    "v_ne",
    "V_NE",
    ISettings::ADJUSTMENT_V_NE,
    100, 0, 300, 1,
  };
  NumericParameter<double> ALPHA_STALL {
    "alpha_stall",
    "α_CRIT",
    ISettings::ADJUSTMENT_ALPHA_STALL,
    15.0, -10.0, 30.0, 0.1,
  };
  NumericParameter<double> ALPHA_STALL_WARNING {
    "alpha_stall_warning",
    "α_CRIT_W",
    ISettings::ADJUSTMENT_ALPHA_STALL_WARNING,
    14.0,
    -10.0, 30.0, 0.1,
  };
  NumericParameter<double> ALPHA_MIN {
    "alpha_min",
    "α_MIN",
    ISettings::ADJUSTMENT_ALPHA_MIN,
    -10.0,
    -10.0, 30.0, 0.1,
  };
  NumericParameter<double> ALPHA_MAX {
    "alpha_max",
    "α_MAX",
    ISettings::ADJUSTMENT_ALPHA_MAX,
    20.0,
    -10.0, 30.0, 0.1,
  };
  NumericParameter<double> ALPHA_X {
    "alpha_x",
    "α_X",
    ISettings::ADJUSTMENT_ALPHA_X,
    12.0,
    -10.0, 30.0, 0.1,
  };
  NumericParameter<double> ALPHA_Y {
    "alpha_y",
    "α_Y",
    ISettings::ADJUSTMENT_ALPHA_X,
    10.0,
    -10.0, 30.0, 0.1,
  };
  NumericParameter<double> ALPHA_REF {
    "alpha_ref",
    "α_REF",
    ISettings::ADJUSTMENT_ALPHA_REF,
    14.0,
    -10.0, 30.0, 0.1,
  };
  NumericParameter<double> BETA_FULL_SCALE {
    "beta_full_scale",
    "β_FS",
    ISettings::ADJUSTMENT_BETA_FULL_SCALE,
    20.0,
    0, 30.0, 5,
  };
  NumericParameter<double> BETA_BIAS {
    "beta_bias",
    "β BIAS",
    ISettings::ADJUSTMENT_BETA_BIAS,
    0.0,
    0, 30.0, 0.1,
  };
  NumericParameter<double> BARO_SETTING {
    "baro_setting",
    "BARO",
    ISettings::ADJUSTMENT_BARO_SETTING,
    29.92,
    25, 35, 0.01,
  };
  NumericParameter<double> BALL_SMOOTHING_FACTOR {
    "ball_smoothing_factor",
    "BALL SMTH",
    ISettings::ADJUSTMENT_BALL_SMOOTHING_FACTOR,
    1.0,
    0.0, 1.0, 0.01,
  };
  NumericParameter<double> VSI_SMOOTHING_FACTOR {
    "vsi_smoothing_factor",
    "VSI SMTH",
    ISettings::ADJUSTMENT_VSI_SMOOTHING_FACTOR,
    1.0,
    0.0, 1.0, 0.01,
  };
  NumericParameter<int> SCREEN_WIDTH {
    "screen_width",
    "DO_NOT_DISPLAY",
    ISettings::ADJUSTMENT_NONE,
    272,
    272, 272, 0,
  };
  NumericParameter<int> SCREEN_HEIGHT {
    "screen_height",
    "DO_NOT_DISPLAY",
    ISettings::ADJUSTMENT_NONE,
    480,
    480, 480, 0,
  };
  BoolParameter SHOW_ALTIMETER {
    "show_altimeter",
    "ALT?",
    ISettings::ADJUSTMENT_SHOW_ALTIMETER,
    true,
  };
  BoolParameter SHOW_LINK_STATUS {
    "show_link_status",
    "LINK?",
    ISettings::ADJUSTMENT_SHOW_LINK_STATUS,
    true,
  };
  BoolParameter SHOW_PROBE_BATTERY_STATUS {
    "show_probe_battery_status",
    "BAT?",
    ISettings::ADJUSTMENT_SHOW_PROBE_BATTERY_STATUS,
    true,
  };
  BoolParameter DECLUTTER {
    "declutter",
    "DCLTR?",
    ISettings::ADJUSTMENT_DECLUTTER,
    false,
  };
  StringSelectionParameter SOUND_SCHEME {
      "sound_scheme",
      "SND",
      ISettings::ADJUSTMENT_SOUND_SCHEME,
      {"stallfence", "flyonspeed"},
      0,
  };
  NumericParameter<double> AUDIO_VOLUME {
    "audio_volume",
    "VOL",
    ISettings::ADJUSTMENT_AUDIO_VOLUME,
    1.0,
    0, 1.0, 0.05,
  };
  StringSelectionParameter SPEED_UNITS {
    "speed_units",
    "SPD",
    ISettings::ADJUSTMENT_SPEED_UNITS,
    {"knots", "mph"},
    0,
  };
  BoolParameter ROTATE_SCREEN {
    "rotate_screen",
    "DO_NOT_DISPLAY",
    ISettings::ADJUSTMENT_NONE,
    false,
  };
  NumericParameter<double> SCREEN_BRIGHTNESS {
    "screen_brightness",
    "BRT",
    ISettings::ADJUSTMENT_SCREEN_BRIGHTNESS,
    1.0,
    0, 1.0, 0.05,
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
      (Parameter*) &BALL_SMOOTHING_FACTOR,
      (Parameter*) &VSI_SMOOTHING_FACTOR,
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
  };
};

} // namespace airball

#endif // AIRBALL_SETTINGS_STORE_H
