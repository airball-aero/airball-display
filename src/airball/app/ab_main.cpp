#include "../model/Settings.h"

#include <iostream>
#include <memory>
#include <pigpio.h>
#include "gflags/gflags.h"

#include "../../framework/Application.h"
#include "../model/IAirballModel.h"
#include "../screen/x11_screen.h"
#include "../model/telemetry/UdpTelemetry.h"
#include "../model/telemetry/FakeTelemetry.h"
#include "../model/Airdata.h"
#include "../model/ISettings.h"
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
DEFINE_uint32(telemetry_udp_port, 30123, "Listening port for UDP telemetry");
DEFINE_string(telemetry_log_path, "airball.log", "File path for log telemetry");

DEFINE_string(sound_device, "hw:0", "ALSA sound device");

DEFINE_string(settings_file_path, "airball-settings.json", "Path to settings file");

DEFINE_string(settings_input_device_path, "", "Path to settings adjustment /dev/input device");

const auto kFrameInterval = std::chrono::milliseconds(0);

namespace airball {

class AirballModel : public IAirballModel {
public:
  AirballModel(std::unique_ptr<IAirdata> airdata, std::unique_ptr<ISettings> settings)
      : airdata_(std::move(airdata)), settings_(std::move(settings)) {}

  [[nodiscard]] const IAirdata* airdata() const override { return airdata_.get(); }
  [[nodiscard]] const ISettings* settings() const override { return settings_.get(); }

private:
  std::unique_ptr<IAirdata> airdata_;
  std::unique_ptr<ISettings> settings_;
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
    return std::make_unique<UdpTelemetry>(FLAGS_telemetry_udp_port);
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


void write_single_gpio(unsigned char bit, unsigned char state) {
  if (bit > 31) {
    exit(-1);
  }
  const uint32_t s = 0x01 << bit;
  if (state) {
    gpioWrite_Bits_0_31_Set(s);
  } else {
    gpioWrite_Bits_0_31_Clear(s);
  }
  volatile uint32_t barrier = gpioRead_Bits_0_31();
}


class AirballApplication : public Application<IAirballModel> {
public:
  AirballApplication() = default;
  ~AirballApplication() override = default;

protected:
  void initialize() override {
    if (gpioInitialise() < 0) {
      exit(-1);
    }
    gpioSetMode(2, PI_OUTPUT);
    write_single_gpio(2, 0);
    write_single_gpio(2, 1);
    write_single_gpio(2, 0);
    setFrameInterval(kFrameInterval);
    auto settings = std::make_unique<Settings>(
        FLAGS_settings_file_path,
        FLAGS_settings_input_device_path,
        eventQueue());
    setScreen(buildScreen(settings.get()));
    setModel(std::make_unique<AirballModel>(
        std::move(std::make_unique<Airdata>(
            eventQueue(),
            std::move(buildTelemetry()),
            settings.get())),
        std::move(settings)));
    setView(std::move(std::make_unique<AirballView>()));
    setSoundMixer(std::make_unique<sound_mixer>(FLAGS_sound_device));
    setSoundScheme(std::make_unique<airball_sound_scheme>());
  }
};

} // namespace airball

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  airball::AirballApplication app;
  app.run();
}
