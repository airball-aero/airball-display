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

  struct SettingsRequest {
  };

  struct CompressedSettings {
    std::string value;
  };

  typedef std::variant<Unknown, Airdata, Battery, SettingsRequest, CompressedSettings> Sample;

  virtual Sample receiveSample() = 0;
  virtual void sendSample(Sample s) = 0;
};

}  // namespace airball

#endif  // AIRBALL_TELEMETRY_TELEMETRY_SAMPLE_H
