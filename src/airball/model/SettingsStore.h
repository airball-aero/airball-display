#ifndef AIRBALL_SETTINGS_STORE_H
#define AIRBALL_SETTINGS_STORE_H

#include "Parameter.h"

namespace airball {

class SettingsStore {
public:
  StringSelectionParameter SPEED_UNITS {
    TelemetryIds::SETTINGS_SPEED_UNITS,
    "speed_units",
    "SPD",
    {"knots", "mph"},
    0,
  };
  SpeedParameter IAS_FULL_SCALE {
    TelemetryIds::SETTINGS_IAS_FULL_SCALE,
    "ias_full_scale",
    "V_FS",
    100, 0, 300, 1,
    "%3.0f",
    &SPEED_UNITS,
  };
  SpeedParameter V_R {
    TelemetryIds::SETTINGS_V_R,
    "v_r",
    "V_R",
    50, 0, 300, 1,
    "%3.0f",
    &SPEED_UNITS,
  };
  SpeedParameter V_FE {
    TelemetryIds::SETTINGS_V_FE,
    "v_fe",
    "V_FE",
    75, 0, 300, 1,
    "%3.0f",
    &SPEED_UNITS,
  };
  SpeedParameter V_NO {
    TelemetryIds::SETTINGS_V_NO,
    "v_no",
    "V_NO",
    100, 0, 300, 1,
    "%3.0f",
    &SPEED_UNITS,
  };
  SpeedParameter V_NE {
    TelemetryIds::SETTINGS_V_NE,
    "v_ne",
    "V_NE",
    100, 0, 300, 1,
    "%3.0f",
    &SPEED_UNITS,
  };
  AngleParameter ALPHA_STALL {
    TelemetryIds::SETTINGS_ALPHA_STALL,
    "alpha_stall",
    "α_CRIT",
    15.0, -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_STALL_WARNING {
    TelemetryIds::SETTINGS_ALPHA_STALL_WARNING,
    "alpha_stall_warning",
    "α_CRIT_W",
    14.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_MIN {
    TelemetryIds::SETTINGS_ALPHA_MIN,
    "alpha_min",
    "α_MIN",
    -10.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_MAX {
    TelemetryIds::SETTINGS_ALPHA_MAX,
    "alpha_max",
    "α_MAX",
    20.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_X {
    TelemetryIds::SETTINGS_ALPHA_X,
    "alpha_x",
    "α_X",
    12.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_Y {
    TelemetryIds::SETTINGS_ALPHA_Y,
    "alpha_y",
    "α_Y",
    10.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter ALPHA_REF {
    TelemetryIds::SETTINGS_ALPHA_REF,
    "alpha_ref",
    "α_REF",
    14.0,
    -10.0, 30.0, 0.1,
    "%4.1f",
  };
  AngleParameter BETA_FULL_SCALE {
    TelemetryIds::SETTINGS_BETA_FULL_SCALE,
    "beta_full_scale",
    "β_FS",
    20.0,
    0, 30.0, 5,
    "%4.1f",
  };
  AngleParameter BETA_BIAS {
    TelemetryIds::SETTINGS_BETA_BIAS,
    "beta_bias",
    "β BIAS",
    0.0,
    0, 30.0, 0.1,
    "%4.1f",
  };
  NumericParameter<double> BARO_SETTING {
    TelemetryIds::SETTINGS_BARO_SETTING,
    "baro_setting",
    "BARO",
    29.92,
    25, 35, 0.01,
    "%5.2f",
  };
  NumericParameter<double> BALL_TIME_CONSTANT {
    TelemetryIds::SETTINGS_BALL_TIME_CONSTANT,
      "ball_time_constant",
      "BALL T",
      0.5,
      0, 1.0, 0.1,
      "%4.2f",
  };
  NumericParameter<double> VSI_TIME_CONSTANT {
    TelemetryIds::SETTINGS_VSI_TIME_CONSTANT,
    "vsi_time_constant",
    "VSI T",
    1.0,
    0.1, 5.0, 0.1,
    "%3.1f",
  };
  NumericParameter<int16_t> SCREEN_WIDTH {
    TelemetryIds::SETTINGS_SCREEN_WIDTH,
    "screen_width",
    "DO_NOT_DISPLAY",
    272,
    272, 272, 0,
    "%d",
  };
  NumericParameter<int16_t> SCREEN_HEIGHT {
    TelemetryIds::SETTINGS_SCREEN_HEIGHT,
    "screen_height",
    "DO_NOT_DISPLAY",
    480,
    480, 480, 0,
    "%d",
  };
  BoolParameter SHOW_ALTIMETER {
    TelemetryIds::SETTINGS_SHOW_ALTIMETER,
    "show_altimeter",
    "ALT?",
    true,
  };
  BoolParameter SHOW_LINK_STATUS {
    TelemetryIds::SETTINGS_SHOW_LINK_STATUS,
    "show_link_status",
    "LINK?",
    true,
  };
  BoolParameter SHOW_PROBE_BATTERY_STATUS {
    TelemetryIds::SETTINGS_SHOW_PROBE_BATTERY_STATUS,
    "show_probe_battery_status",
    "BAT?",
    true,
  };
  BoolParameter DECLUTTER {
    TelemetryIds::SETTINGS_DECLUTTER,
    "declutter",
    "DCLTR?",
    false,
  };
  StringSelectionParameter SOUND_SCHEME {
    TelemetryIds::SETTINGS_SOUND_SCHEME,
      "sound_scheme",
      "SND",
      {"stallfence", "flyonspeed"},
      0,
  };
  NumericParameter<double> AUDIO_VOLUME {
    TelemetryIds::SETTINGS_AUDIO_VOLUME,
      "audio_volume",
      "VOL",
      1.0,
      0, 1.0, 0.05,
      "%4.2f",
  };
  BoolParameter ROTATE_SCREEN {
    TelemetryIds::SETTINGS_ROTATE_SCREEN,
      "rotate_screen",
      "DO_NOT_DISPLAY",
      false,
  };
  NumericParameter<double> SCREEN_BRIGHTNESS {
    TelemetryIds::SETTINGS_SCREEN_BRIGHTNESS,
    "screen_brightness",
    "BRT",
    1.0,
    0, 1.0, 0.05,
    "%4.2f",
  };
  BoolParameter SHOW_NUMERIC_AIRSPEED {
    TelemetryIds::SETTINGS_SHOW_NUMERIC_AIRSPEED,
      "show_numeric_airspeed",
      "SPD?",
      true,
  };
  NumericParameter<double> Q_CORRECTION_FACTOR {
    TelemetryIds::SETTINGS_Q_CORRECTION_FACTOR,
      "q_correction_factor",
      "Q_COR",
      1.0,
      0.5, 1.5, 0.05,
      "%4.2f",
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
      (Parameter*) &Q_CORRECTION_FACTOR,
  };
};

} // namespace airball

#endif // AIRBALL_SETTINGS_STORE_H
