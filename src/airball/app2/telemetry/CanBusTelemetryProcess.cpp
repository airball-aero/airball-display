#include "CanBusTelemetryProcess.h"

namespace airball {

CanBusTelemetryProcess::CanBusTelemetryProcess(const std::string& serialDevice) {
  enqueue([this]() {
    loop();
  });
  start();
}

void CanBusTelemetryProcess::send(std::vector<Parameter> data) {
  enqueue([this, data]() {
    // ???
  });
}

} // namespace airball
