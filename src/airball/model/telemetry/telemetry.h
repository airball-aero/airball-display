#ifndef AIRBALL_TELEMETRY_TELEMETRY_SAMPLE_H
#define AIRBALL_TELEMETRY_TELEMETRY_SAMPLE_H

#include <string>

namespace airball {

class Telemetry {
public:
  enum Type {
    UNKNOWN = 0,
    AIRDATA = 1,
    BATTERY = 2,
  };

  struct Airdata {
    double alpha;
    double beta;
    double q;
    double p;
    double t;
  };

  struct Battery {
    double battery_voltage;
    double battery_current;
    double battery_capacity_pct;
  };

  struct Sample {
    Type type;
    int sequence;
    union {
      Airdata airdata;
      Battery battery;
    };
  };

  static Sample parse(const std::string& message);
};

}  // namespace airball

#endif  // AIRBALL_TELEMETRY_TELEMETRY_SAMPLE_H
