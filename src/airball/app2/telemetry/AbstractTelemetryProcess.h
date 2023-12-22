#ifndef SRC_AIRBALL_APP2_TELEMETRY_ABSTRACT_TELEMETRY_PROCESS_H
#define SRC_AIRBALL_APP2_TELEMETRY_ABSTRACT_TELEMETRY_PROCESS_H

#include "ITelemetryProcess.h"
#include "../../util/Process.h"

namespace airball {

class AbstractTelemetryProcess : public Process, public ITelemetryProcess {
public:
  void addTelemetryCallback(std::function<void(Parameter p)> cb) override;

protected:
  void publish(std::vector<Parameter> parameters);

private:
  std::vector<std::function<void(Parameter p)>> cbs_;
};

} // namespace airball

#endif // SRC_AIRBALL_APP2_TELEMETRY_ABSTRACT_TELEMETRY_PROCESS_H
