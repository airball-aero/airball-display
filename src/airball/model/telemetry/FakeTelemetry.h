#ifndef AIRBALL_TELEMETRY_FAKE_TELEMETRY_H
#define AIRBALL_TELEMETRY_FAKE_TELEMETRY_H

#include <memory>
#include <vector>
#include <deque>

#include "ITelemetry.h"

namespace airball {

class FakeTelemetry : public ITelemetry {
public:
  FakeTelemetry();
  ~FakeTelemetry() = default;

  Message receive() override;
  void send(Message s) override;
  void send(std::vector<Message> s) override;

private:
  void make_airdata();

  uint32_t seq_counter_;
  std::deque<Message> ready_to_receive_;
};

}

#endif // AIRBALL_TELEMETRY_FAKE_TELEMETRY_H
