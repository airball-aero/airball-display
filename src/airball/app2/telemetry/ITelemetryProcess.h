#ifndef SRC_AIRBALL_APP2_TELEMETRY_TELEMETRY_PROCESS_H
#define SRC_AIRBALL_APP2_TELEMETRY_TELEMETRY_PROCESS_H

#include <functional>
#include <inttypes.h>

namespace airball {

class ITelemetryProcess {
public:
  class Parameter {
    uint16_t id;
    uint8_t data[8];
  };

  virtual void addTelemetryCallback(std::function<void(Parameter p)> cb) = 0;
  virtual void send(std::vector<Parameter> data) = 0;
};

}

#endif //SRC_AIRBALL_APP2_TELEMETRY_TELEMETRY_PROCESS_H
