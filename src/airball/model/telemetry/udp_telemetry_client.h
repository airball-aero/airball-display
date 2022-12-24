#ifndef AIRBALL_ESP32_TELEMETRY_CLIENT_H
#define AIRBALL_ESP32_TELEMETRY_CLIENT_H

#include "telemetry_client.h"
#include "udp_packet_reader.h"

namespace airball {

class UdpTelemetryClient : public TelemetryClient {
public:
  /**
   * Create a client to the Airball probe via an Internet (likely Wi-Fi)
   * connection to its well-known host name and port number.
   */
  explicit UdpTelemetryClient();
  ~UdpTelemetryClient();

  Telemetry::Sample get() override;

private:
  UdpPacketReader reader_;
};

}  // namespace airball

#endif //AIRBALL_ESP32_TELEMETRY_CLIENT_H






