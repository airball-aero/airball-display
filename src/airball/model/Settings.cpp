#include "Settings.h"

#include <rapidjson/document.h>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <iostream>
#include <mutex>

#include "../util/file_write_watch.h"
#include "../util/one_shot_timer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "SettingsStore.h"

namespace airball {

const auto kInputDelay = std::chrono::milliseconds(5000);

// A SettingsEventSource handles all the asynchronous operations to listen for HID
// events from input devices, timeouts, and all that stuff. It is responsible for
// notifying the Settings object when stuff happens, on the UI event loop so that
// the Settings code can stop worrying and just focus on its main job.
class SettingsEventSource {
public:
  SettingsEventSource(const std::string& settingsFilePath,
                      const std::string& inputDevicePath,
                      IEventQueue* eventQueue,
                      Settings* settings)
      : settingsFilePath_(settingsFilePath),
        inputDevicePath_(inputDevicePath),
        eventQueue_(eventQueue),
        settings_(settings) {
    fileWatchThread_ = std::thread([this]() {
      file_write_watch w(settingsFilePath_);
      while (true) {
        w.next_event();
        eventQueue_->enqueue([&](){ settings_->load(); });
      }
    });
    if (!inputDevicePath_.empty()) {
      inputThread_ = std::thread([this]() {
        while (true) {
          int f = open(inputDevicePath_.c_str(), O_RDONLY);
          if (f < 0) {
            std::this_thread::sleep_for(kInputDelay);
            continue;
          }
          input_event e = {0};
          while (read(f, &e, sizeof(e)) == sizeof(e)) {
            switch (e.type) {
              case EV_KEY:
                switch (e.code) {
                  case KEY_VOLUMEUP:
                    if (e.value == 0) {
                      eventQueue_->enqueue([this]() { settings_->hidIncrement(); });
                      restartCancelTimer();
                    }
                    break;
                  case KEY_VOLUMEDOWN:
                    if (e.value == 0) {
                      eventQueue_->enqueue([this]() { settings_->hidDecrement(); });
                      restartCancelTimer();
                    }
                    break;
                  case KEY_PLAYPAUSE:
                    switch (e.value) {
                      case 0:
                        eventQueue_->enqueue([this]() { settings_->hidAdjustReleased(); });
                        restartCancelTimer();
                        break;
                      case 1:
                        eventQueue_->enqueue([this]() { settings_->hidAdjustPressed(); });
                        restartCancelTimer();
                        break;
                      default:
                        break;
                    }
                    break;
                  default:
                    break;
                }
              default:
                break;
            }
          }
          close(f);
        }
      });
    }
  }

  void restartCancelTimer() {
    std::lock_guard<std::mutex> lock(cancelTimerMu_);
    cancelTimer_.reset(new OneShotTimer(kInputDelay, [this]() {
      eventQueue_->enqueue([this]() {
        settings_->hidTimerExpired();
      });
    }));
  }

private:
  const std::string settingsFilePath_;
  const std::string inputDevicePath_;
  IEventQueue* eventQueue_;
  Settings* settings_;

  std::thread fileWatchThread_;
  std::thread inputThread_;

  std::mutex cancelTimerMu_;
  std::unique_ptr<OneShotTimer> cancelTimer_;
};

Settings::Settings(const std::string& settingsFilePath,
                   const std::string& inputDevicePath,
                   IEventQueue *eventQueue)
    : path_(settingsFilePath),
      loaded_(false),
      eventQueue_(eventQueue),
      adjustment_(ADJUSTMENT_NONE) {
  settingsEventSource_ = std::make_unique<SettingsEventSource>(
          settingsFilePath,
          inputDevicePath,
          eventQueue,
          this);
  store_ = std::make_unique<SettingsStore>();
  load();
}

Settings::~Settings() {}

void Settings::load() {
  std::ifstream f;
  f.open(path_);
  std::stringstream s;
  s << f.rdbuf();
  f.close();

  rapidjson::Document d;
  d.Parse(s.str().c_str());
  for (Parameter *p : store_->ALL_PARAMS) {
    p->load(d);
  }

  loaded_ = true;
}

void Settings::save() {
  if (!loaded_) {
    return;
  }

  rapidjson::Document d;
  d.SetObject();
  for (Parameter *p : store_->ALL_PARAMS) {
    p->save(d);
  }

  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);

  std::ofstream f;
  f.open(path_);
  f << buffer.GetString();
  f.flush();
  f.close();
}

double Settings::v_full_scale() const {
  return store_->V_FULL_SCALE.get();
}

double Settings::v_r() const {
  return store_->V_R.get();
}

double Settings::v_fe() const {
  return store_->V_FE.get();
}

double Settings::v_no() const {
  return store_->V_NO.get();
}

double Settings::v_ne() const {
  return store_->V_NE.get();
}

double Settings::alpha_stall() const {
  return store_->ALPHA_STALL.get();
}

double Settings::alpha_stall_warning() const {
  return store_->ALPHA_STALL_WARNING.get();
}

double Settings::alpha_min() const {
  return store_->ALPHA_MIN.get();
}

double Settings::alpha_max() const {
  return store_->ALPHA_MAX.get();
}

double Settings::alpha_x() const {
  return store_->ALPHA_X.get();
}

double Settings::alpha_y() const {
  return store_->ALPHA_Y.get();
}

double Settings::alpha_ref() const {
  return store_->ALPHA_REF.get();
}

double Settings::beta_full_scale() const {
  return store_->BETA_FULL_SCALE.get();
}

double Settings::beta_bias() const {
  return store_->BETA_BIAS.get();
}

double Settings::baro_setting() const {
  return store_->BARO_SETTING.get();
}

double Settings::ball_smoothing_factor() const {
  return store_->BALL_SMOOTHING_FACTOR.get();
}

double Settings::vsi_smoothing_factor() const {
  return store_->VSI_SMOOTHING_FACTOR.get();
}

int Settings::screen_width() const {
  return store_->SCREEN_WIDTH.get();
}

int Settings::screen_height() const {
  return store_->SCREEN_HEIGHT.get();
}

bool Settings::show_altimeter() const {
  return store_->SHOW_ALTIMETER.get();
}

bool Settings::show_link_status() const {
  return store_->SHOW_LINK_STATUS.get();
}

bool Settings::show_probe_battery_status() const {
  return store_->SHOW_PROBE_BATTERY_STATUS.get();
}

bool Settings::declutter() const {
  return store_->DECLUTTER.get();
}

std::string Settings::sound_scheme() const {
  return store_->SOUND_SCHEME.get();
}

double Settings::audio_volume() const {
  return store_->AUDIO_VOLUME.get();
}

std::string Settings::speed_units() const {
  return store_->SPEED_UNITS.get();
}

bool Settings::rotate_screen() const {
  return store_->ROTATE_SCREEN.get();
}

double Settings::screen_brightness() const {
  return store_->SCREEN_BRIGHTNESS.get();
}

Settings::Adjustment Settings::adjustment() const {
  return adjustment_;
}

void Settings::startAdjusting() {
  if (adjustment_ == ADJUSTMENT_NONE) {
    if (show_altimeter()) {
      adjustment_ = ADJUSTMENT_BARO_SETTING;
    } else {
      adjustment_ = ADJUSTMENT_SCREEN_BRIGHTNESS;
    }
  }
}

void Settings::nextAdjustment() {
  if (adjustment_ == ADJUSTMENT_NONE) {
    startAdjusting();
    return;
  }
  switch (adjustment_) {
    case ISettings::ADJUSTMENT_BARO_SETTING:
      adjustment_ = ADJUSTMENT_SCREEN_BRIGHTNESS;
      break;
    case ISettings::ADJUSTMENT_SCREEN_BRIGHTNESS:
      adjustment_ = ADJUSTMENT_AUDIO_VOLUME;
      break;
    case ISettings::ADJUSTMENT_AUDIO_VOLUME:
    default:
      adjustment_ = ADJUSTMENT_NONE;
      break;
  }
}

void Settings::hidIncrement() {
  startAdjusting();
  switch (adjustment_) {
    case ISettings::ADJUSTMENT_BARO_SETTING:
      store_->BARO_SETTING.set(store_->BARO_SETTING.get() + 0.01);
      break;
    case ISettings::ADJUSTMENT_SCREEN_BRIGHTNESS:
      store_->SCREEN_BRIGHTNESS.set(std::min(store_->SCREEN_BRIGHTNESS.get() + 0.05, 1.0));
      break;
    case ISettings::ADJUSTMENT_AUDIO_VOLUME:
      store_->AUDIO_VOLUME.set(std::min(store_->AUDIO_VOLUME.get() + 0.05, 1.0));
      break;
    default:
      return;
  }
  save();
}

void Settings::hidDecrement() {
  startAdjusting();
  switch (adjustment_) {
    case ISettings::ADJUSTMENT_BARO_SETTING:
      store_->BARO_SETTING.set(store_->BARO_SETTING.get() - 0.01);
      break;
    case ISettings::ADJUSTMENT_SCREEN_BRIGHTNESS:
      store_->SCREEN_BRIGHTNESS.set(std::max(store_->SCREEN_BRIGHTNESS.get() - 0.05, 0.0));
      break;
    case ISettings::ADJUSTMENT_AUDIO_VOLUME:
      store_->AUDIO_VOLUME.set(std::max(store_->AUDIO_VOLUME.get() - 0.05, 0.0));
      break;
    default:
      return;
  }
  save();
}

void Settings::hidAdjustPressed() {
  nextAdjustment();
}

void Settings::hidAdjustReleased() {}

void Settings::hidTimerExpired() {
  adjustment_ = ADJUSTMENT_NONE;
}

} // namespace airball

