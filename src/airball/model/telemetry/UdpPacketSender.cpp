#include "UdpPacketSender.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>

namespace airball {

UdpPacketSender::UdpPacketSender(std::string sendAddress, int sendPort)
    : sendAddress_(sendAddress), sendPort_(sendPort), fd_(0) {
  open();
}

bool UdpPacketSender::open() {
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ < 0) {
    return false;
  }
  return true;
}

UdpPacketSender::~UdpPacketSender() {
  if (fd_ != 0) {
    close(fd_);
  }
}

void UdpPacketSender::send(std::string str) {
  sockaddr_in servaddr;
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(sendAddress_.c_str());
  servaddr.sin_port = htons(sendPort_);
  sendto(fd_, str.c_str(), str.size() + 1, 0,
         (sockaddr*)&servaddr, sizeof(servaddr));
}

}  // namespace airball