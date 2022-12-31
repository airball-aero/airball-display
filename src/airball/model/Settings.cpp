#include "Settings.h"

#include <rapidjson/document.h>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <csignal>

#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <iostream>

#include "../util/file_write_watch.h"

namespace airball {

template<class T>
class Parameter {
public:
  const char *name;
  const T initial;

  T get(const rapidjson::Document &doc) const {
    if (doc.HasMember(name)) {
      return get_impl(doc);
    }
    return initial;
  }

  T get_impl(const rapidjson::Document &doc) const;
};

template<>
int Parameter<int>::get_impl(const rapidjson::Document &doc) const {
  return doc[name].GetInt();
}

template<>
double Parameter<double>::get_impl(const rapidjson::Document &doc) const {
  return doc[name].GetDouble();
}

template<>
bool Parameter<bool>::get_impl(const rapidjson::Document &doc) const {
  return doc[name].GetBool();
}

template<>
std::string Parameter<std::string>::get_impl(const rapidjson::Document &doc) const {
  return doc[name].GetString();
}

constexpr Parameter<double> V_FULL_SCALE = {
    .name="ias_full_scale",
    .initial=100,
};

constexpr Parameter<double> V_R = {
    .name="v_r",
    .initial=100,
};

constexpr Parameter<double> V_FE = {
    .name="v_fe",
    .initial=100,
};

constexpr Parameter<double> V_NO = {
    .name="v_no",
    .initial=100,
};

constexpr Parameter<double> V_NE = {
    .name="v_ne",
    .initial=100,
};

constexpr Parameter<double> ALPHA_STALL = {
    .name="alpha_stall",
    .initial=15.0,
    };

constexpr Parameter<double> ALPHA_STALL_WARNING = {
    .name="alpha_stall_warning",
    .initial=14.0,
    };

constexpr Parameter<double> ALPHA_MIN = {
    .name="alpha_min",
    .initial=0.0,
};

constexpr Parameter<double> ALPHA_MAX = {
    .name="alpha_max",
    .initial=20.0,
};

constexpr Parameter<double> ALPHA_X = {
    .name="alpha_x",
    .initial=12.0,
};

constexpr Parameter<double> ALPHA_Y = {
    .name="alpha_y",
    .initial=10.0,
};

constexpr Parameter<double> ALPHA_REF = {
    .name="alpha_ref",
    .initial=14.0,
};

constexpr Parameter<double> BETA_FULL_SCALE = {
    .name="beta_full_scale",
    .initial=20.0,
};

constexpr Parameter<double> BETA_BIAS = {
    .name="beta_bias",
    .initial=0.0,
};

constexpr Parameter<double> BARO_SETTING = {
    .name="baro_setting",
    .initial=29.92,
};

constexpr Parameter<double> BALL_SMOOTHING_FACTOR = {
    .name="ball_smoothing_factor",
    .initial=1.0,
};

constexpr Parameter<double> VSI_SMOOTHING_FACTOR = {
    .name="vsi_smoothing_factor",
    .initial=1.0,
};

constexpr Parameter<int> SCREEN_WIDTH = {
    .name="screen_width",
    .initial=272,
};

constexpr Parameter<int> SCREEN_HEIGHT = {
    .name="screen_height",
    .initial=480,
};

constexpr Parameter<bool> SHOW_ALTIMETER = {
    .name="show_altimeter",
    .initial=true,
};

constexpr Parameter<bool> SHOW_LINK_STATUS = {
    .name="show_link_status",
    .initial=true,
};

constexpr Parameter<bool> SHOW_PROBE_BATTERY_STATUS = {
    .name="show_probe_battery_status",
    .initial=true,
};

constexpr Parameter<bool> DECLUTTER = {
    .name="declutter",
    .initial=false,
};

const Parameter<std::string> SOUND_SCHEME = {
    .name="sound_scheme",
    .initial="flyonspeed",
};

const Parameter<double> AUDIO_VOLUME = {
    .name="audio_volume",
    .initial=1.0,
};

const Parameter<std::string> SPEED_UNITS = {
    .name="speed_units",
    .initial="mph",
};

const Parameter<bool> ROTATE_SCREEN = {
    .name="rotate_screen",
    .initial=false,
};

const auto kInputDelay = std::chrono::milliseconds(100);

Settings::Settings(const std::string& settingsFilePath,
                   const std::string& inputDevicePath,
                   IEventQueue *eventQueue)
    : path_(settingsFilePath), inputDevicePath_(inputDevicePath), eventQueue_(eventQueue) {
  load();
  fileWatchThread_ = std::thread([&]() {
    file_write_watch w(path_);
    while (true) {
      w.next_event();
      eventQueue->enqueue([&](){ load(); });
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
                    eventQueue_->enqueue([this]() { hidIncrement(); });
                  }
                  break;
                case KEY_VOLUMEDOWN:
                  if (e.value == 0) {
                    eventQueue_->enqueue([this]() { hidDecrement(); });
                  }
                  break;
                case KEY_PLAYPAUSE:
                  switch (e.value) {
                    case 0:
                      eventQueue_->enqueue([this]() { hidAdjustReleased(); });
                      break;
                    case 1:
                      eventQueue_->enqueue([this]() { hidAdjustPressed(); });
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

Settings::~Settings() {
  // TODO: Not correct
  fileWatchThread_.join();
  inputThread_.join();
}

void Settings::load_str(std::string str) {
  document_.Parse("{}");
  document_.Parse(str.c_str());
}

void Settings::load() {
  std::ifstream f;
  f.open(path_);
  std::stringstream s;
  s << f.rdbuf();
  f.close();
  load_str(s.str());
}

template<class T>
T Settings::get_value(const Parameter<T> *p) const {
  return p->get(document_);
}

double Settings::v_full_scale() const {
  return get_value(&V_FULL_SCALE);
}

double Settings::v_r() const {
  return get_value(&V_R);
}

double Settings::v_fe() const {
  return get_value(&V_FE);
}

double Settings::v_no() const {
  return get_value(&V_NO);
}

double Settings::v_ne() const {
  return get_value(&V_NE);
}

double Settings::alpha_stall() const {
  return get_value(&ALPHA_STALL);
}

double Settings::alpha_stall_warning() const {
  return get_value(&ALPHA_STALL_WARNING);
}

double Settings::alpha_min() const {
  return get_value(&ALPHA_MIN);
}

double Settings::alpha_max() const {
  return get_value(&ALPHA_MAX);
}

double Settings::alpha_x() const {
  return get_value(&ALPHA_X);
}

double Settings::alpha_y() const {
  return get_value(&ALPHA_Y);
}

double Settings::alpha_ref() const {
  return get_value(&ALPHA_REF);
}

double Settings::beta_full_scale() const {
  return get_value(&BETA_FULL_SCALE);
}

double Settings::beta_bias() const {
  return get_value(&BETA_BIAS);
}

double Settings::baro_setting() const {
  return get_value(&BARO_SETTING);
}

double Settings::ball_smoothing_factor() const {
  return get_value(&BALL_SMOOTHING_FACTOR);
}

double Settings::vsi_smoothing_factor() const {
  return get_value(&VSI_SMOOTHING_FACTOR);
}

int Settings::screen_width() const {
  return get_value(&SCREEN_WIDTH);
}

int Settings::screen_height() const {
  return get_value(&SCREEN_HEIGHT);
}

bool Settings::show_altimeter() const {
  return get_value(&SHOW_ALTIMETER);
}

bool Settings::show_link_status() const {
  return get_value(&SHOW_LINK_STATUS);
}

bool Settings::show_probe_battery_status() const {
  return get_value(&SHOW_PROBE_BATTERY_STATUS);
}

bool Settings::declutter() const {
  return get_value(&DECLUTTER);
}

std::string Settings::sound_scheme() const {
  return get_value(&SOUND_SCHEME);
}

double Settings::audio_volume() const {
  return get_value(&AUDIO_VOLUME);
}

std::string Settings::speed_units() const {
  return get_value(&SPEED_UNITS);
}

bool Settings::rotate_screen() const {
  return get_value(&ROTATE_SCREEN);
}

void Settings::hidIncrement() {
  std::cout << "hidIncrement" << std::endl;
}

void Settings::hidDecrement() {
  std::cout << "hidDecrement" << std::endl;
}

void Settings::hidAdjustPressed() {
  std::cout << "hidAdjustPressed" << std::endl;
}

void Settings::hidAdjustReleased() {
  std::cout << "hidAdjustReleased" << std::endl;
}

} // namespace airball

