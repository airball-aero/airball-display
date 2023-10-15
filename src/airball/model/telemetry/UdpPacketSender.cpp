#include "UdpPacketSender.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

namespace airball {

UdpPacketSender::UdpPacketSender(std::string broadcast_ip, int broadcast_port)
    : broadcast_ip_(broadcast_ip), broadcast_port_(broadcast_port), socket_fd_(0) {
  open();
}

bool UdpPacketSender::open() {
  socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_fd_ < 0) {
    return false;
  }

  int broadcast_permission = 1;
  int rc = setsockopt(socket_fd_,
                      SOL_SOCKET,
                      SO_BROADCAST,
                      (void *) &broadcast_permission,
                      sizeof(broadcast_permission));
  if (rc < 0) {
    return false;
  }

  memset(&broadcast_addr_, 0, sizeof(broadcast_addr_));
  broadcast_addr_.sin_family = AF_INET;
  broadcast_addr_.sin_addr.s_addr = inet_addr(broadcast_ip_.c_str());
  broadcast_addr_.sin_port = htons(broadcast_port_);

  return true;
}

UdpPacketSender::~UdpPacketSender() {
  if (socket_fd_ != 0) {
    close(socket_fd_);
  }
}

void UdpPacketSender::send(std::string str) {
  sendto(socket_fd_,
         str.c_str(),
         str.size(),
         0,
         (struct sockaddr *)&broadcast_addr_,
         sizeof(broadcast_addr_));
}

}  // namespace airball