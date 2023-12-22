#include "UdpTelemetryProcess.h"

#include <cstring>

namespace airball {

std::string parametersToPacket(const std::vector<ITelemetryProcess::Parameter>& parameters) {
  std::string result(parameters.size() * sizeof(ITelemetryProcess::Parameter), 0);
  for (int i = 0; i < parameters.size(); i++) {
    std::memcpy(
        &(result.data()[sizeof(ITelemetryProcess::Parameter) * i]),
        &(parameters[i]),
        sizeof(ITelemetryProcess::Parameter));
  }
  return result;
}

std::vector<ITelemetryProcess::Parameter> packetToParameters(const std::string& packet) {
  std::vector<ITelemetryProcess::Parameter> result;
  int n = packet.length() / sizeof(ITelemetryProcess::Parameter);
  ITelemetryProcess::Parameter p;
  for (int i = 0; i < n; i++) {
    std::memcpy(
        &p,
        &(packet.data()[sizeof(ITelemetryProcess::Parameter) * i]),
        sizeof(ITelemetryProcess::Parameter));
    result.push_back(p);
  }
  return result;
}

std::chrono::duration<int, std::milli> kTimeout(50);

UdpTelemetryProcess::UdpTelemetryProcess(int port, const std::string& receiveInterface, const std::string& broadcastIp)
    : reader_(port, receiveInterface),
      sender_(broadcastIp, port) {
  enqueue([this]() {
    loop();
  });
  start();
}

void UdpTelemetryProcess::send(std::vector<Parameter> data) {
  enqueue([this, data]() {
    sender_.send(parametersToPacket(data));
  });
}

void UdpTelemetryProcess::loop() {
  if (reader_.poll(kTimeout)) {
    publish(packetToParameters(reader_.read()));
  }
  enqueue([this]() {
    loop();
  });
}

} // namespace airball
