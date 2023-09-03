#ifndef AIRBALL_TELEMETRY_TELEMETRY_SAMPLE_H
#define AIRBALL_TELEMETRY_TELEMETRY_SAMPLE_H

#include <string>
#include <variant>

namespace airball {

class ITelemetry {
public:
  struct Unknown {
  };

  struct Airdata {
    unsigned long sequence;
    double alpha;
    double beta;
    double q;
    double p;
    double t;
  };

  struct Battery {
    unsigned long sequence;
    double battery_voltage;
    double battery_current;
    double battery_capacity_pct;
  };

  typedef std::variant<Unknown, Airdata, Battery> Sample;

  virtual Sample get() = 0;

protected:
  static Sample parse(const std::string& message);
};

}  // namespace airball

#endif  // AIRBALL_TELEMETRY_TELEMETRY_SAMPLE_H
