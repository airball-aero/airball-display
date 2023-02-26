#ifndef AIRBALL_MODEL_I_SETTINGS_H
#define AIRBALL_MODEL_I_SETTINGS_H

#include <string>

namespace airball {

class ISettings {
public:
  /**
   * @return the display setting, full scale indicated air speed. (Knots)
   */
  virtual double ias_full_scale() const = 0;

  /**
   * @return the rotation indicated air speed (Vr). (Knots)
   */
  virtual double v_r() const = 0;

  /**
   * @return the maximum flap extension indicated air speed (Vfe). (Knots)
   */
  virtual double v_fe() const = 0;

  /**
   * @return the maximum normal operating indicated air speed (Vno). (Knots)
   */
  virtual double v_no() const = 0;

  /**
   * @return the never-exceed indicated air speed (Vne). (Knots)
   */
  virtual double v_ne() const = 0;

  /**
   * @return the stall angle of attack. (Degrees)
   */
  virtual double alpha_stall() const = 0;

  /**
   * @return the stall warning angle of attack. (Degrees)
   */
  virtual double alpha_stall_warning() const = 0;

  /**
   * @return the display setting, minimum angle of attack shown. (Degrees)
   */
  virtual double alpha_min() const = 0;

  /**
   * @return the display setting, maximum angle of attack shown. (Degrees)
   */
  virtual double alpha_max() const = 0;

  /**
   * @return the maximum climb rate angle of attack. (Degrees)
   */
  virtual double alpha_x() const = 0;

  /**
   * @return the maximum climb angle angle of attack. (Degrees)
   */
  virtual double alpha_y() const = 0;

  /**
   * @return the reference stabilized approach angle of attack. (Degrees)
   */
  virtual double alpha_ref() const = 0;

  /**
   * @return  full scale angle of yaw. (Degrees)
   */
  virtual double beta_full_scale() const = 0;

  /**
   * @return the display setting, constant bias to angle of yaw. (Degrees)
   */
  virtual double beta_bias() const = 0;

  /**
   * @return the current barometer setting. (Inches of mercury)
   */
  virtual double baro_setting() const = 0;

  /**
   * @return the ball smoothing factor.
   */
  virtual double ball_smoothing_factor() const = 0;

  /**
   * @return the VSI smoothing factor.
   */
  virtual double vsi_smoothing_factor() const = 0;

  /**
   * @return the screen width.
   */
  virtual int screen_width() const = 0;

  /**
   * @return the screen height.
   */
  virtual int screen_height() const = 0;

  /**
   * @return whether to display the altimeter.
   */
  virtual bool show_altimeter() const = 0;

  /**
   * @return whether to display the wireless link status.
   */
  virtual bool show_link_status() const = 0;

  /**
   * @return whether to display the probe battery status.
   */
  virtual bool show_probe_battery_status() const = 0;

  /**
   * @return whether to declutter the displayed annotations.
   */
  virtual bool declutter() const = 0;

  /**
   * @return the name of the sound scheme to use.
   */
  virtual std::string sound_scheme() const = 0;

  /**
   * @return the desired audio volume, from 0.0 (none) to 1.0 (full).
   */
  virtual double audio_volume() const = 0;

  /**
   * @return the speed units, which can be "mph" or "knots".
   */
   virtual std::string speed_units() const = 0;

  /**
   * @return whether the material displayed on the screen should be rotated 90 degrees.
   */
  virtual bool rotate_screen() const = 0;

  /**
   * @return the desired sreen brightness, from 0.0 (full dark) to 1.0 (full bright).
   */
  virtual double screen_brightness () const = 0;

  /**
   * @return whether to show a numeric airspeed in the ball.
   */
   virtual bool show_numeric_airspeed() const = 0;

   enum Adjustment {
     ADJUSTMENT_NONE = 0,
     ADJUSTMENT_IAS_FULL_SCAlE = 1,
     ADJUSTMENT_V_R = 2,
     ADJUSTMENT_V_FE = 3,
     ADJUSTMENT_V_NO = 4,
     ADJUSTMENT_V_NE = 5,
     ADJUSTMENT_ALPHA_STALL = 6,
     ADJUSTMENT_ALPHA_STALL_WARNING = 7,
     ADJUSTMENT_ALPHA_MIN = 8,
     ADJUSTMENT_ALPHA_MAX = 9,
     ADJUSTMENT_ALPHA_X = 10,
     ADJUSTMENT_ALPHA_Y = 11,
     ADJUSTMENT_ALPHA_REF = 12,
     ADJUSTMENT_BETA_FULL_SCALE = 13,
     ADJUSTMENT_BETA_BIAS = 14,
     ADJUSTMENT_BARO_SETTING = 15,
     ADJUSTMENT_BALL_SMOOTHING_FACTOR = 16,
     ADJUSTMENT_VSI_SMOOTHING_FACTOR = 17,
     ADJUSTMENT_SHOW_ALTIMETER = 18,
     ADJUSTMENT_SHOW_LINK_STATUS = 19,
     ADJUSTMENT_SHOW_PROBE_BATTERY_STATUS = 20,
     ADJUSTMENT_DECLUTTER = 21,
     ADJUSTMENT_SOUND_SCHEME = 22,
     ADJUSTMENT_AUDIO_VOLUME = 23,
     ADJUSTMENT_SPEED_UNITS = 24,
     ADJUSTMENT_SCREEN_BRIGHTNESS = 25,
     ADJUSTMENT_SHOW_NUMERIC_AIRSPEED = 26,
   };

  virtual Adjustment adjustment() const = 0;

  virtual std::string adjustmentDisplayName() const = 0;
  virtual std::string adjustmentDisplayValue() const = 0;
};

} // namespace airball

#endif // AIRBALL_MODEL_I_SETTINGS_H
