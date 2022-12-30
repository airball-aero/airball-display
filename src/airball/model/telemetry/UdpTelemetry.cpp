#include "UdpTelemetry.h"

namespace airball {

UdpTelemetry::UdpTelemetry(int udpPort)
    : reader_(udpPort) {}

ITelemetry::Sample
UdpTelemetry::get() {
  return parse(reader_.readLine());
}

} // airball