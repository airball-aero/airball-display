#ifndef SRC_AIRBALL_APP2_TELEMETRY_UDP_TELEMETRY_PROCESS_H
#define SRC_AIRBALL_APP2_TELEMETRY_UDP_TELEMETRY_PROCESS_H

#include "AbstractTelemetryProcess.h"
#include "../../model/telemetry/UdpPacketReader.h"
#include "../../model/telemetry/UdpPacketSender.h"

namespace airball {

class UdpTelemetryProcess : public AbstractTelemetryProcess {
public:
  UdpTelemetryProcess(int port, const std::string& receiveInterface, const std::string& broadcastIp);

  void send(std::vector<Parameter> data) override;

private:
  void loop();

  UdpPacketReader reader_;
  UdpPacketSender sender_;
};

} // namespace airball

#endif //SRC_AIRBALL_APP2_TELEMETRY_UDP_TELEMETRY_PROCESS_H
