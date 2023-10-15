#include "../model/Settings.h"

#include <iostream>
#include <memory>
#include "gflags/gflags.h"

#include "../../framework/Application.h"
#include "../model/IAirballModel.h"
#include "../screen/x11_screen.h"
#include "../model/telemetry/UdpTelemetry.h"
#include "../model/telemetry/FakeTelemetry.h"
#include "../model/Airdata.h"
#include "../view/AirballView.h"
#include "../screen/image_screen.h"
#include "../sound_mixer/sound_mixer.h"
#include "../sound_scheme/airball_sound_scheme.h"

#ifdef AIRBALL_BCM2835
#include "../screen/st7789vi_screen.h"
#endif

const std::string kScreenX11 = "x11";
const std::string kScreenImage = "image";
#ifdef AIRBALL_BCM2835
const std::string kScreenSt7789vi = "st7789vi";
DEFINE_string(screen, kScreenX11, "Screen implementation (x11, image, st7789vi)");
#else
DEFINE_string(screen, kScreenX11, "Screen implementation (x11, image)");
#endif

const std::string kTelemetryUdp = "udp";
const std::string kTelemetryLog = "log";
const std::string kTelemetryFake = "fake";
DEFINE_string(telemetry, kTelemetryFake, "Telemetry (udp, log, fake)");
DEFINE_string(telemetry_udp_bcast, "192.168.4.255", "Broadcast address for UDP telemetry");
DEFINE_uint32(telemetry_udp_port, 30123, "IP port for UDP telemetry");
DEFINE_string(telemetry_udp_interface, "wlan0", "Interface for UDP telemetry");
DEFINE_string(telemetry_log_path, "airball.log", "File path for log telemetry");

DEFINE_string(sound_device, "hw:0", "ALSA sound device");

DEFINE_string(settings_file_path, "airball-settings.json", "Path to settings file");

DEFINE_string(settings_input_device_path, "", "Path to settings adjustment /dev/input device");

const auto kFrameInterval = std::chrono::milliseconds(0);

namespace airball {

class AirballModel : public IAirballModel {
public:
  AirballModel(IAirdata* airdata, ISettings* settings)
      : airdata_(airdata), settings_(settings) {}

  [[nodiscard]] const IAirdata* airdata() const override { return airdata_; }
  [[nodiscard]] const ISettings* settings() const override { return settings_; }

private:
  IAirdata* airdata_;
  ISettings* settings_;
};

std::unique_ptr<IScreen> buildScreen(const ISettings* settings) {
  if (FLAGS_screen == kScreenX11) {
    return std::make_unique<X11Screen>(settings->screen_width(), settings->screen_height());
  }
  if (FLAGS_screen == kScreenImage) {
    return std::make_unique<ImageScreen>(settings->screen_width(), settings->screen_height());
  }
  #ifdef AIRBALL_BCM2835
  if (FLAGS_screen == kScreenSt7789vi) {
    return std::make_unique<ST7789VIScreen>();
  }
  #endif
  std::cerr << "Unsupported screen option " << FLAGS_screen << std::endl;
  exit(-1);
}

std::unique_ptr<ITelemetry> buildTelemetry() {
  if (FLAGS_telemetry == kTelemetryUdp) {
    return std::make_unique<UdpTelemetry>(FLAGS_telemetry_udp_bcast,
                                          FLAGS_telemetry_udp_port,
                                          FLAGS_telemetry_udp_interface);
  }
  if (FLAGS_telemetry == kTelemetryLog) {
    std::cerr << "Log telemetry is not yet supported, sorry!" << std::endl;
  }
  if (FLAGS_telemetry == kTelemetryFake) {
    return std::make_unique<FakeTelemetry>();
  }
  std::cerr << "Unsupported telemetry option " << FLAGS_telemetry << std::endl;
  exit(-1);
}


class AirballApplication : public Application<IAirballModel> {
public:
  AirballApplication() = default;
  ~AirballApplication() override = default;

protected:
  void initialize() override {
    setFrameInterval(kFrameInterval);
    telemetry_ = buildTelemetry();
    settings_ = std::make_unique<Settings>(
        FLAGS_settings_file_path,
        FLAGS_settings_input_device_path,
        eventQueue(),
        [this](ITelemetry::Sample sample) {
          telemetry_->sendSample(sample);
        });
    setScreen(buildScreen(settings_.get()));
    airdata_ = std::make_unique<Airdata>(settings_.get());
    setModel(std::make_unique<AirballModel>(
        airdata_.get(),
        settings_.get()));
    setView(std::move(std::make_unique<AirballView>()));
    setSoundMixer(std::make_unique<sound_mixer>(FLAGS_sound_device));
    setSoundScheme(std::make_unique<airball_sound_scheme>());

    telemetry_read_thread_ = std::thread([&]() {
      while (true) {
        ITelemetry::Sample s = telemetry_->receiveSample();
        if (std::holds_alternative<ITelemetry::Airdata>(s)) {
          eventQueue()->enqueue([this, s]() {
            airdata_->update(std::get<ITelemetry::Airdata>(s));
          });
        }
        if (std::holds_alternative<ITelemetry::CompressedSettings>(s)) {
          eventQueue()->enqueue([this, s]() {
            settings_->acceptCompressedSettings(std::get<ITelemetry::CompressedSettings>(s));
          });
        }
        if (std::holds_alternative<ITelemetry::SettingsRequest>(s)) {
          eventQueue()->enqueue([this, s]() {
            settings_->acceptSettingsRequest(std::get<ITelemetry::SettingsRequest>(s));
          });
        }
      }
    });
  }

private:
  std::unique_ptr<Settings> settings_;
  std::unique_ptr<IAirdata> airdata_;
  std::unique_ptr<ITelemetry> telemetry_;
  std::thread telemetry_read_thread_;
};

} // namespace airball

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  airball::AirballApplication app;
  app.run();
}
