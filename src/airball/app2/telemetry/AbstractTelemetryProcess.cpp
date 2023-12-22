#include "AbstractTelemetryProcess.h"

namespace airball {

void AbstractTelemetryProcess::addTelemetryCallback(std::function<void(Parameter p)> cb) {
  enqueue([this, cb]() {
    cbs_.push_back(cb);
  });
}

void AbstractTelemetryProcess::publish(std::vector<Parameter> parameters) {
  for (auto i : cbs_) {
    for (auto p : parameters) {
      i(p);
    }
  }
}

} // namespace airball
