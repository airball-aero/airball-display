#ifndef AIRBALL_TELEMETRY_UDP_PACKET_SENDER_H
#define AIRBALL_TELEMETRY_UDP_PACKET_SENDER_H

#include <string>
#include <netinet/in.h>

namespace airball {

class UdpPacketSender {
public:
  explicit UdpPacketSender(std::string broadcast_ip, int broadcast_port);
  ~UdpPacketSender();

  void send(std::string str);

private:
  bool open();

  std::string broadcast_ip_;
  int broadcast_port_;
  int socket_fd_;
  struct sockaddr_in broadcast_addr_;
};

}  // namespace airball

#endif  // AIRBALL_TELEMETRY_UDP_PACKET_SENDER_H
