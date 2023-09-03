#ifndef AIRBALL_TELEMETRY_UDP_PACKET_SENDER_H
#define AIRBALL_TELEMETRY_UDP_PACKET_SENDER_H

#include <string>

namespace airball {

class UdpPacketSender {
public:
  explicit UdpPacketSender(std::string sendAddress, int sendPort);
  ~UdpPacketSender();

  void send(std::string str);

private:
  bool open();

  std::string sendAddress_;
  int sendPort_;
  int fd_;
};

}  // namespace airball

#endif  // AIRBALL_TELEMETRY_UDP_PACKET_SENDER_H
