#ifndef AIRBALL_SETTINGS_STORE_H
#define AIRBALL_SETTINGS_STORE_H

#include <vector>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace airball {

class Parameter {
public:
  virtual void load(const rapidjson::Document &doc) = 0;
  virtual void save(rapidjson::Document &doc) const = 0;
};

template<class T>
class TypedParameter : public Parameter {
public:
  TypedParameter(const std::string& name, const T& initial)
      : name_(name), value_(initial) {}

  T get() const { return value_; }
  void set(const T& value) { value_ = value; }

  void load(const rapidjson::Document &doc) override {
    if (doc.HasMember(name_.c_str())) {
      loadImpl(doc);
    }
  }

  void save(rapidjson::Document &doc) const override {
    saveImpl(doc);
  }

private:
  void loadImpl(const rapidjson::Document &doc);
  void saveImpl(rapidjson::Document &doc) const;

  const std::string name_;
  T value_;
};

template<>
void TypedParameter<int>::loadImpl(const rapidjson::Document &doc) {
  value_ = doc[name_.c_str()].GetInt();
}

template<>
void TypedParameter<int>::saveImpl(rapidjson::Document &doc) const {
  doc[name_.c_str()].Set(value_);
}

template<>
void TypedParameter<double>::loadImpl(const rapidjson::Document &doc) {
  value_ = doc[name_.c_str()].GetDouble();
}

template<>
void TypedParameter<double>::saveImpl(rapidjson::Document &doc) const {
  doc[name_.c_str()].Set(value_);
}

template<>
void TypedParameter<bool>::loadImpl(const rapidjson::Document &doc) {
  value_ = doc[name_.c_str()].GetBool();
}

template<>
void TypedParameter<bool>::saveImpl(rapidjson::Document &doc) const {
  doc[name_.c_str()].Set(value_);
}

template<>
void TypedParameter<std::string>::loadImpl(const rapidjson::Document &doc) {
  value_ = doc[name_.c_str()].GetString();
}

template<>
void TypedParameter<std::string>::saveImpl(rapidjson::Document &doc) const {
  doc[name_.c_str()].Set(value_.c_str());
}

class SettingsStore {
public:
  TypedParameter<double> V_FULL_SCALE {"ias_full_scale",100};
  TypedParameter<double> V_R {"v_r", 100};
  TypedParameter<double> V_FE {"v_fe", 100};
  TypedParameter<double> V_NO {"v_no",100};
  TypedParameter<double> V_NE {"v_ne", 100};
  TypedParameter<double> ALPHA_STALL {"alpha_stall", 15.0};
  TypedParameter<double> ALPHA_STALL_WARNING {"alpha_stall_warning", 14.0};
  TypedParameter<double> ALPHA_MIN {"alpha_min", 0.0};
  TypedParameter<double> ALPHA_MAX {"alpha_max", 20.0};
  TypedParameter<double> ALPHA_X {"alpha_x", 12.0};
  TypedParameter<double> ALPHA_Y {"alpha_y", 10.0};
  TypedParameter<double> ALPHA_REF {"alpha_ref", 14.0};
  TypedParameter<double> BETA_FULL_SCALE {"beta_full_scale", 20.0};
  TypedParameter<double> BETA_BIAS {"beta_bias", 0.0};
  TypedParameter<double> BARO_SETTING {"baro_setting", 29.92};
  TypedParameter<double> BALL_SMOOTHING_FACTOR {"ball_smoothing_factor", 1.0};
  TypedParameter<double> VSI_SMOOTHING_FACTOR {"vsi_smoothing_factor", 1.0};
  TypedParameter<int> SCREEN_WIDTH {"screen_width", 272};
  TypedParameter<int> SCREEN_HEIGHT {"screen_height", 480};
  TypedParameter<bool> SHOW_ALTIMETER {"show_altimeter", true};
  TypedParameter<bool> SHOW_LINK_STATUS {"show_link_status", true};
  TypedParameter<bool> SHOW_PROBE_BATTERY_STATUS {"show_probe_battery_status", true};
  TypedParameter<bool> DECLUTTER {"declutter", false};
  TypedParameter<std::string> SOUND_SCHEME {"sound_scheme", "flyonspeed"};
  TypedParameter<double> AUDIO_VOLUME {"audio_volume", 1.0};
  TypedParameter<std::string> SPEED_UNITS {"speed_units", "mph"};
  TypedParameter<bool> ROTATE_SCREEN {"rotate_screen", false};
  TypedParameter<double> SCREEN_BRIGHTNESS {"screen_brightness", 1.0};

  const std::vector<Parameter *> ALL_PARAMS = {
      (Parameter*) &V_FULL_SCALE,
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
