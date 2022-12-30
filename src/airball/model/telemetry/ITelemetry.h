#ifndef AIRBALL_TELEMETRY_TELEMETRY_SAMPLE_H
#define AIRBALL_TELEMETRY_TELEMETRY_SAMPLE_H

#include <string>

namespace airball {

class ITelemetry {
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
    unsigned long sequence;
    union {
      Airdata airdata;
      Battery battery;
    };
  };

  virtual Sample get() = 0;

protected:
  static Sample parse(const std::string& message);
};

}  // namespace airball

#endif  // AIRBALL_TELEMETRY_TELEMETRY_SAMPLE_H
