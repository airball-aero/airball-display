#ifndef SRC_AIRBALL_APP2_TELEMETRY_CAN_BUS_TELEMETRY_PROCESS_H
#define SRC_AIRBALL_APP2_TELEMETRY_CAN_BUS_TELEMETRY_PROCESS_H

#include "AbstractTelemetryProcess.h"
#include "../../model/telemetry/UdpPacketReader.h"
#include "../../model/telemetry/UdpPacketSender.h"

namespace airball {

class CanBusTelemetryProcess : public AbstractTelemetryProcess {
public:
  CanBusTelemetryProcess(const std::string& serialDevice);

  void send(std::vector<Parameter> data) override;

private:
  void loop();

};

} // namespace airball

#endif // SRC_AIRBALL_APP2_TELEMETRY_CAN_BUS_TELEMETRY_PROCESS_H
