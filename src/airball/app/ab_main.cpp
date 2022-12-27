#include <iostream>
#include <memory>
#include "gflags/gflags.h"

#include "../../framework/Application.h"
#include "../model/IAirballModel.h"
#include "../screen/x11_screen.h"
#include "../model/telemetry/ILineReader.h"
#include "../model/telemetry/udp_packet_reader.h"
#include "../model/Airdata.h"
#include "../model/ISettings.h"
#include "../model/Settings.h"

const std::string kScreenX11 = "x11";
const std::string kScreenSt7789vi = "st7789vi";
DEFINE_string(screen, kScreenX11, "Screen implementation (x11, st7789vi)");

const std::string kLineReaderUdp = "udp";
const std::string kLineReaderLog = "log";
DEFINE_string(line_reader, kLineReaderUdp, "Line reader (udp, log)");
DEFINE_uint32(line_reader_udp_port, 30123, "Listening port for UDP line reader");
DEFINE_string(line_reader_log_path, "airball.log", "File path for log line reader");

DEFINE_string(settings_path, "airball-settings.json", "Path to settings file");

const auto kFrameInterval = std::chrono::duration<double, std::milli>(50);

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
    return std::make_unique<X11Screen>(400, 300);
  }
  #ifdef AIRBALL_BCM2835
  if (FLAGS_screen = kScreenSt7789vi) {
    return new St7789viScreen(400, 300);
  }
  #endif

  exit(-1);
}

std::unique_ptr<ILineReader> buildLineReader() {
  if (FLAGS_line_reader == kLineReaderUdp) {
    return std::make_unique<UdpPacketReader>(
        FLAGS_line_reader_udp_port);
  }
  if (FLAGS_line_reader == kLineReaderLog) {
    exit(-1);
  }
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
    auto lineReader = buildLineReader();
    setModel(std::make_unique<AirballModel>(
        std::move(std::make_unique<Airdata>(
            eventQueue(),
            std::move(lineReader),
            settings.get())),
        std::move(settings)));
  }
};

} // namespace airball

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  airball::AirballApplication app;
  app.run();
}