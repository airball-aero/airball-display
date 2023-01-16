#ifndef AIRBALL_SETTINGS_H
#define AIRBALL_SETTINGS_H

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "ISettings.h"
#include <thread>
#include "../../framework/IEventQueue.h"

namespace airball {

class SettingsEventSource;
class SettingsStore;

class Settings : public ISettings {
public:
  Settings(const std::string& settingsFilePath,
           const std::string& inputDevicePath,
           IEventQueue *eventQueue);
  virtual ~Settings();

  double v_full_scale() const override;
  double v_r() const override;
  double v_fe() const override;
  double v_no() const override;
  double v_ne() const override;
  double alpha_stall() const override;
  double alpha_stall_warning() const override;
  double alpha_min() const override;
  double alpha_max() const override;
  double alpha_x() const override;
  double alpha_y() const override;
  double alpha_ref() const override;
  double beta_full_scale() const override;
  double beta_bias() const override;
  double baro_setting() const override;
  double ball_smoothing_factor() const override;
  double vsi_smoothing_factor() const override;
  int screen_width() const override;
  int screen_height() const override;
  bool show_altimeter() const override;
  bool show_link_status() const override;
  bool show_probe_battery_status() const override;
  bool declutter() const override;
  std::string sound_scheme() const override;
  double audio_volume() const override;
  std::string speed_units() const override;
  bool rotate_screen() const override;
  double screen_brightness() const override;

  Adjustment adjustment() const override;

  void hidIncrement();
  void hidDecrement();
  void hidAdjustPressed();
  void hidAdjustReleased();
  void hidTimerExpired();

  void load();
  void save();

private:
  void startAdjusting();
  void nextAdjustment();

  std::string path_;
  bool loaded_;
  std::unique_ptr<SettingsEventSource> settingsEventSource_;
  std::unique_ptr<SettingsStore> store_;
  IEventQueue* eventQueue_;
  Adjustment adjustment_;
};

} // namespace airball

#endif // AIRBALL_SETTINGS_H
