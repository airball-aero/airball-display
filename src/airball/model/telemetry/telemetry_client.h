#ifndef AIRBALL_TELEMETRY_TELEMETRY_CLIENT_H
#define AIRBALL_TELEMETRY_TELEMETRY_CLIENT_H

#include "telemetry.h"

namespace airball {

/**
 * A client that receives Telemetry from an Airball probe.
 */
class TelemetryClient {
public:
  /**
   * Blocks until a Sample is available, then returns it.
   */
  virtual Telemetry::Sample get() = 0;
};

}  // namespace airball

#endif // AIRBALL_TELEMETRY_TELEMETRY_CLIENT_H
