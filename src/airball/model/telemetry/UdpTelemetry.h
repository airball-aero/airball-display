#ifndef SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H
#define SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H

#include <memory>
#include <chrono>
#include <deque>

#include "ITelemetry.h"
#include "UdpPacketReader.h"
#include "UdpPacketSender.h"

namespace airball {

class UdpTelemetry : public ITelemetry {
public:
  UdpTelemetry(std::string broadcastAddress, int udpPort, std::string networkInterface);
  ~UdpTelemetry() = default;

  Message receive() override;
  void send(Message m) override;
  void send(std::vector<Message> m) override;

private:
  UdpPacketReader reader_;
  UdpPacketSender sender_;
  std::deque<Message> incoming_;
};

} // airball

#endif // SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H
