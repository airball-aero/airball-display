#include <iostream>
#include "UdpTelemetry.h"

namespace airball {

UdpTelemetry::UdpTelemetry(int udpPort)
    : reader_(udpPort),
      packet_count_(0),
      max_delta_(std::chrono::steady_clock::duration::zero()),
      previous_time_(std::chrono::steady_clock::now()) {}

ITelemetry::Sample
UdpTelemetry::get() {
  auto r = parse(reader_.readLine());

  if (packet_count_ == 100) {
    packet_count_ = 0;
    std::cout << "Max milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(max_delta_).count() << std::endl;
    max_delta_ = std::chrono::steady_clock::duration::zero();
  }

  auto now = std::chrono::steady_clock::now();
  max_delta_ = std::max(max_delta_, now - previous_time_);
  previous_time_ = now;
  packet_count_++;

  return r;
}

} // airball