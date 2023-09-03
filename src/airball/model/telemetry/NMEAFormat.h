#ifndef AIRBALL_TELEMETRY_NMEA_FORMAT_H
#define AIRBALL_TELEMETRY_NMEA_FORMAT_H

#include "ITelemetry.h"

namespace airball {

class NMEAFormat {
public:
  static ITelemetry::Sample unmarshal(const std::string& message);
  static std::string marshal(ITelemetry::Sample s);
};

}  // namespace airball

#endif  // AIRBALL_TELEMETRY_NMEA_FORMAT_H
