#ifndef SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H
#define SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H

#include <memory>

#include "ITelemetry.h"
#include "UdpPacketReader.h"

namespace airball {

class UdpTelemetry : public ITelemetry {
public:
  UdpTelemetry(int udpPort);
  ~UdpTelemetry() = default;

  Sample get() override;

private:
  UdpPacketReader reader_;
};

} // airball

#endif // SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H
