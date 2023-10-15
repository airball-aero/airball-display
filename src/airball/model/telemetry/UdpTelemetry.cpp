#include "UdpTelemetry.h"

#include "NMEAFormat.h"

namespace airball {

UdpTelemetry::UdpTelemetry(std::string broadcastAddress, int udpPort, std::string networkInterface)
    : reader_(udpPort, networkInterface),
      sender_(broadcastAddress, udpPort) {}

ITelemetry::Sample
UdpTelemetry::receiveSample() {
  return NMEAFormat::unmarshal(reader_.read());
}

void UdpTelemetry::sendSample(ITelemetry::Sample s) {
  sender_.send(NMEAFormat::marshal(s));
}

} // airball