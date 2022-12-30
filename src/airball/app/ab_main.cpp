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
#include "../model/ISettings.h"
#include "../view/AirballView.h"
#include "../screen/image_screen.h"

const std::string kScreenX11 = "x11";
const std::string kScreenSt7789vi = "st7789vi";
const std::string kScreenImage = "image";
DEFINE_string(screen, kScreenX11, "Screen implementation (x11, image,st7789vi)");

const std::string kTelemetryUdp = "udp";
const std::string kTelemetryLog = "log";
const std::string kTelemetryFake = "fake";
DEFINE_string(telemetry, kTelemetryFake, "Telemetry (udp, log, fake)");
DEFINE_uint32(telemetry_udp_port, 30123, "Listening port for UDP line reader");
DEFINE_string(telemetry_log_path, "airball.log", "File path for log line reader");

DEFINE_string(settings_path, "airball-settings.json", "Path to settings file");

const auto kFrameInterval = std::chrono::milliseconds(50);

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

std::unique_ptr<IScreen> buildScreen() {
  if (FLAGS_screen == kScreenX11) {
    return std::make_unique<X11Screen>(600, 800);
  }
  if (FLAGS_screen == kScreenImage) {
    return std::make_unique<ImageScreen>(400, 300);
  }
  #ifdef AIRBALL_BCM2835
  if (FLAGS_screen = kScreenSt7789vi) {
    return new St7789viScreen(400, 300);
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

class AirballApplication : public Application<IAirballModel> {
public:
  AirballApplication() = default;
  ~AirballApplication() override = default;

protected:
  void initialize() override {
    setFrameInterval(kFrameInterval);
    setScreen(buildScreen());
    auto settings = std::make_unique<Settings>(
        FLAGS_settings_path,
        eventQueue());
    setModel(std::make_unique<AirballModel>(
        std::move(std::make_unique<Airdata>(
            eventQueue(),
            std::move(buildTelemetry()),
            settings.get())),
        std::move(settings)));
    setView(std::move(std::make_unique<AirballView>()));
  }
};

} // namespace airball

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  airball::AirballApplication app;
  app.run();
}