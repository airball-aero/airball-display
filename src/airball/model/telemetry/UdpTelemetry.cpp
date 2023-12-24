#include "UdpTelemetry.h"

#include <vector>

#include "MessageBlockFormat.h"

namespace airball {

UdpTelemetry::UdpTelemetry(std::string broadcastAddress, int udpPort, std::string networkInterface)
    : reader_(udpPort, networkInterface),
      sender_(broadcastAddress, udpPort) {}

ITelemetry::Message
UdpTelemetry::receive() {
  while (incoming_.empty()) {
    auto r = MessageBlockFormat::unmarshal(reader_.read());
    incoming_.assign(r.begin(), r.end());
  }
  auto m = incoming_.front();
  incoming_.pop_front();
  return m;
}

void UdpTelemetry::send(ITelemetry::Message m) {
  send(std::vector<ITelemetry::Message> { m });
}

void UdpTelemetry::send(std::vector<ITelemetry::Message> m) {
  sender_.send(MessageBlockFormat::marshal(m));
}

} // namespace airball