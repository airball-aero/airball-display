#ifndef AIRBALL_SETTINGS_H
#define AIRBALL_SETTINGS_H

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "ISettings.h"
#include <thread>
#include "../../framework/IEventQueue.h"
#include "telemetry/ITelemetry.h"

namespace airball {

class SettingsEventSource;
class SettingsStore;
class Parameter;

class Settings : public ISettings {
public:
  Settings(const std::string& settingsFilePath,
           const std::string& inputDevicePath,
           IEventQueue *eventQueue,
           std::function<void(ITelemetry::Message)> sendMessage);
  virtual ~Settings();

  double ias_full_scale() const override;
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
  double ball_time_constant() const override;
  double vsi_time_constant() const override;
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
  bool show_numeric_airspeed() const override;
  double q_correction_factor() const override;

  bool adjusting() const override;

  std::string adjustmentDisplayName() const override;
  std::string adjustmentDisplayValue() const override;

  void hidIncrement();
  void hidDecrement();
  void hidAdjustPressed();
  void hidAdjustReleased();
  void hidCancelTimerFired();
  void hidLongPressTimerFired();

  void loadFromFile();
  void saveToFile();

  void acceptMessage(ITelemetry::Message);

  enum AdjustmentKnobState {
    UNKNOWN = 0,
    DISCONNECTED = 1,
    CONNECTED = 2,
  };

  void setAdjustmentKnobState(AdjustmentKnobState);

private:
  void loadFromString(std::string);
  std::string saveToString();

  void maybeSendSettings();

  void startAdjustingShallow();
  void nextAdjustment();

  void buildParamsVectors();

  std::string path_;
  std::function<void(ITelemetry::Message)> sendMessage_;
  bool loadedFromFile_;
  std::unique_ptr<SettingsEventSource> settingsEventSource_;
  std::unique_ptr<SettingsStore> store_;

  std::vector<Parameter*> adjustmentParamsShallow_;
  std::vector<Parameter*> adjustmentParamsDeep_;
  std::vector<Parameter*>* currentAdjustingVector_;
  size_t currentAdjustingIndex_;

  AdjustmentKnobState adjustmentKnobState_;
};

} // namespace airball

#endif // AIRBALL_SETTINGS_H
