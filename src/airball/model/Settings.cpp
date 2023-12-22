#include "Settings.h"

#include <fcntl.h>
#include <unistd.h>
#include <mutex>

#include <linux/input.h>
#include <linux/input-event-codes.h>

#include <rapidjson/document.h>
#include "rapidjson/writer.h"

#include "../util/atomic_store.h"
#include "../util/file_write_watch.h"
#include "../util/one_shot_timer.h"
#include "SettingsStore.h"

namespace airball {

const auto kCancelDelay = std::chrono::milliseconds(5000);
const auto kLongPressDelay = std::chrono::milliseconds (2000);

const size_t kStorePageSize =   256;
const size_t kStoreBankSize = 16384;

// A SettingsEventSource handles all the asynchronous operations to listen for HID
// events from input devices, timeouts, and all that stuff. It is responsible for
// notifying the Settings object when stuff happens, on the UI event loop so that
// the Settings code can stop worrying and just focus on its main job.
class SettingsEventSource {
public:
  SettingsEventSource(const std::string &settingsFilePath,
                      const std::string &inputDevicePath,
                      IEventQueue *eventQueue,
                      Settings *settings)
      : settingsFilePath_(settingsFilePath),
        inputDevicePath_(inputDevicePath),
        eventQueue_(eventQueue),
        settings_(settings) {
    fileWatchThread_ = std::thread([this]() {
      file_write_watch w(settingsFilePath_);
      while (true) {
        w.next_event();
        eventQueue_->enqueue([&]() { settings_->loadFromFile(); });
      }
    });
    inputThread_ = std::thread([this]() {
      while (true) {
        int f = open(inputDevicePath_.c_str(), O_RDONLY);
        if (f < 0) {
          eventQueue_->enqueue([this]() { settings_->setAdjustmentKnobState(Settings::DISCONNECTED); });
          std::this_thread::sleep_for(kCancelDelay);
          continue;
        }
        eventQueue_->enqueue([this]() { settings_->setAdjustmentKnobState(Settings::CONNECTED); });
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
                      stopLongPressTimer();
                      break;
                    case 1:
                      eventQueue_->enqueue([this]() { settings_->hidAdjustPressed(); });
                      restartCancelTimer();
                      restartLongPressTimer();
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

  void restartCancelTimer() {
    std::lock_guard<std::mutex> lock(cancelTimerMu_);
    cancelTimer_.reset(new OneShotTimer(kCancelDelay, [this]() {
      eventQueue_->enqueue([this]() {
        settings_->hidCancelTimerFired();
      });
    }));
  }

  void restartLongPressTimer() {
    longPressTimer_.reset(new OneShotTimer(kLongPressDelay, [this]() {
      eventQueue_->enqueue([this]() {
        settings_->hidLongPressTimerFired();
      });
    }));
  }

  void stopLongPressTimer() {
    longPressTimer_.reset(nullptr);
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
  std::unique_ptr<OneShotTimer> longPressTimer_;
};

Settings::Settings(const std::string& settingsFilePath,
                   const std::string& inputDevicePath,
                   IEventQueue *eventQueue,
                   std::function<void(ITelemetry::Message)> sendMessage)
    : path_(settingsFilePath),
      sendMessage_(sendMessage),
      loadedFromFile_(false),
      currentAdjustingVector_(nullptr),
      currentAdjustingIndex_(0),
      adjustmentKnobState_(UNKNOWN) {
  settingsEventSource_ = std::make_unique<SettingsEventSource>(
          settingsFilePath,
          inputDevicePath,
          eventQueue,
          this);
  store_ = std::make_unique<SettingsStore>();
  buildParamsVectors();
  loadFromFile();
}

void Settings::buildParamsVectors() {
  adjustmentParamsShallow_ = {
      &store_->BARO_SETTING,
      &store_->SCREEN_BRIGHTNESS,
      &store_->AUDIO_VOLUME,
      &store_->SHOW_ALTIMETER,
      &store_->SHOW_NUMERIC_AIRSPEED,
  };

  adjustmentParamsDeep_ = {
    &store_->IAS_FULL_SCALE,
    &store_->V_R,
    &store_->V_FE,
    &store_->V_NO,
    &store_->V_NE,
    &store_->ALPHA_STALL,
    &store_->ALPHA_STALL_WARNING,
    &store_->ALPHA_MIN,
    &store_->ALPHA_MAX,
    &store_->ALPHA_X,
    &store_->ALPHA_Y,
    &store_->ALPHA_REF,
    &store_->BETA_FULL_SCALE,
    &store_->BETA_BIAS,
    &store_->BALL_TIME_CONSTANT,
    &store_->VSI_TIME_CONSTANT,
    &store_->DECLUTTER,
    &store_->SOUND_SCHEME,
    &store_->SPEED_UNITS,
    &store_->Q_CORRECTION_FACTOR,
  };
}

Settings::~Settings() {}

void Settings::loadFromFile() {
  AtomicStore s(path_);
  s.initialize(kStorePageSize, kStoreBankSize);
  loadFromString(s.read_payload());
  loadedFromFile_ = true;
}

void Settings::saveToFile() {
  if (loadedFromFile_) {
    AtomicStore s(path_);
    s.initialize(kStorePageSize, kStoreBankSize);
    s.write_payload(saveToString());
  }
}

void Settings::acceptMessage(ITelemetry::Message message) {
  if (message.id == TelemetryIds::SETTINGS_REQUEST) {
    maybeSendSettings();
  } else {
    for (auto p: store_->ALL_PARAMS) {
      if (p->message_id() == message.id) {
        p->fromMessage(message);
      }
    }
  }
}

void Settings::setAdjustmentKnobState(AdjustmentKnobState s) {
  // The first time we find out about a change of state, we respond:
  // - If we are the settings leader, we transmit our settings.
  // - If we are a settings follower, we ask for settings from others.
  if (s != adjustmentKnobState_) {
    adjustmentKnobState_ = s;
    if (adjustmentKnobState_ == DISCONNECTED) {
      sendMessage_(ITelemetry::Message {
        .id = TelemetryIds::SETTINGS_REQUEST,
        .data = { 0 },
      });
    } else {
      maybeSendSettings();
    }
  }
}

void Settings::loadFromString(std::string s) {
  rapidjson::Document d;
  d.SetObject();
  d.Parse(s.c_str());
  for (Parameter *p : store_->ALL_PARAMS) {
    p->load(d);
  }
}

std::string Settings::saveToString() {
  rapidjson::Document d;
  d.SetObject();
  for (Parameter *p : store_->ALL_PARAMS) {
    p->save(d);
  }

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);

  return buffer.GetString();
}

void Settings::maybeSendSettings() {
  if (adjustmentKnobState_ == CONNECTED) {
    for (auto p : store_->ALL_PARAMS) {
      sendMessage_(p->toMessage());
    }
  }
}

double Settings::ias_full_scale() const {
  return store_->IAS_FULL_SCALE.get();
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

double Settings::ball_time_constant() const {
  return store_->BALL_TIME_CONSTANT.get();
}

double Settings::vsi_time_constant() const {
  return store_->VSI_TIME_CONSTANT.get();
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

bool Settings::show_numeric_airspeed() const {
  return store_->SHOW_NUMERIC_AIRSPEED.get();
}

double Settings::q_correction_factor() const {
  return store_->Q_CORRECTION_FACTOR.get();
}

bool Settings::adjusting() const {
  return (currentAdjustingVector_ != nullptr);
}

std::string Settings::adjustmentDisplayName() const  {
  if (currentAdjustingVector_ == nullptr) {
    return "";
  }
  return (*currentAdjustingVector_)[currentAdjustingIndex_]->display_name();
}

std::string Settings::adjustmentDisplayValue() const {
  if (currentAdjustingVector_ == nullptr) {
    return "";
  }
  return (*currentAdjustingVector_)[currentAdjustingIndex_]->display_value();
}

void Settings::startAdjustingShallow() {
  if (currentAdjustingVector_ == nullptr) {
    currentAdjustingVector_ = &adjustmentParamsShallow_;
    currentAdjustingIndex_ = 0;
  }
}

void Settings::nextAdjustment() {
  if (currentAdjustingVector_ == nullptr) {
    startAdjustingShallow();
  } else {
    currentAdjustingIndex_ = (currentAdjustingIndex_ + 1) % currentAdjustingVector_->size();
  }
}

void Settings::hidIncrement() {
  startAdjustingShallow();
  (*currentAdjustingVector_)[currentAdjustingIndex_]->increment();
  saveToFile();
  maybeSendSettings();
}

void Settings::hidDecrement() {
  startAdjustingShallow();
  (*currentAdjustingVector_)[currentAdjustingIndex_]->decrement();
  saveToFile();
  maybeSendSettings();  
}

void Settings::hidAdjustPressed() {
  nextAdjustment();
}

void Settings::hidAdjustReleased() {}

void Settings::hidCancelTimerFired() {
  if (currentAdjustingVector_ == &adjustmentParamsShallow_) {
    currentAdjustingVector_ = nullptr;
    currentAdjustingIndex_ = 0;
  }
}

void Settings::hidLongPressTimerFired() {
  if (currentAdjustingVector_ == nullptr || currentAdjustingVector_ == &adjustmentParamsShallow_) {
    currentAdjustingVector_ = &adjustmentParamsDeep_;
    currentAdjustingIndex_ = 0;
  } else if (currentAdjustingVector_ == &adjustmentParamsDeep_) {
    currentAdjustingVector_ = &adjustmentParamsShallow_;
    currentAdjustingIndex_ = 0;
  }
}

} // namespace airball

