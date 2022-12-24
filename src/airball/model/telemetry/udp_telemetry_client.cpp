#include "udp_telemetry_client.h"

namespace airball {

static constexpr int kAirballUdpPort = 30123;

UdpTelemetryClient::UdpTelemetryClient()
    : reader_(kAirballUdpPort) {
  reader_.open();
}

UdpTelemetryClient::~UdpTelemetryClient() {};

Telemetry::Sample UdpTelemetryClient::get() {
  while (true) {
    try {
      return Telemetry::parse(reader_.read_packet());
    } catch (const std::exception &e) { }
  }
}

} // namespace airball