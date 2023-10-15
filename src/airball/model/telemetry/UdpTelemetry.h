#ifndef SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H
#define SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H

#include <memory>
#include <chrono>

#include "ITelemetry.h"
#include "UdpPacketReader.h"
#include "UdpPacketSender.h"

namespace airball {

class UdpTelemetry : public ITelemetry {
public:
  UdpTelemetry(std::string broadcastAddress, int udpPort, std::string networkInterface);
  ~UdpTelemetry() = default;

  Sample receiveSample() override;
  void sendSample(Sample s) override;

private:
  UdpPacketReader reader_;
  UdpPacketSender sender_;
};

} // airball

#endif // SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H
