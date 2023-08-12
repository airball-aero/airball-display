#ifndef SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H
#define SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H

#include <memory>
#include <chrono>

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

  bool initialized_;
  std::chrono::steady_clock::duration max_delta_;
  std::chrono::steady_clock::time_point previous_time_;
  int packet_count_;
};

} // airball

#endif // SRC_AIRBALL_MODEL_TELEMETRY_UDPTELEMETRY_H
