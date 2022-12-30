#ifndef AIRBALL_TELEMETRY_FAKE_TELEMETRY_CLIENT_H
#define AIRBALL_TELEMETRY_FAKE_TELEMETRY_CLIENT_H

#include <memory>

#include "ITelemetry.h"

namespace airball {

class FakeTelemetry : public ITelemetry {
public:
  FakeTelemetry();
  ~FakeTelemetry() = default;

  Sample get() override;

private:
  unsigned long seq_counter_;
};

}

#endif  // AIRBALL_TELEMETRY_FAKE_TELEMETRY_CLIENT_H