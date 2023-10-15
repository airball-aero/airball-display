#include "UdpPacketReader.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <sys/ioctl.h>
#include <bits/ioctls.h>

struct in_addr getMyIpAddress(const std::string& receive_interface) {
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  struct ifreq ifr;
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name, receive_interface.c_str(), IFNAMSIZ-1);
  ioctl(fd, SIOCGIFADDR, &ifr);
  close(fd);
  return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
}

namespace airball {

UdpPacketReader::UdpPacketReader(int receive_port, const std::string& receive_interface)
    : receive_port_(receive_port),
      receive_interface_(receive_interface),
      socket_fd_(0) {
  memset(receive_buffer_, 0, sizeof(receive_buffer_));
  open();
}

UdpPacketReader::~UdpPacketReader() {
  if (socket_fd_ != 0) {
    close(socket_fd_);
  }
}

bool UdpPacketReader::open() {
  my_address_ = getMyIpAddress(receive_interface_);

  socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_fd_ < 0) {
    return false;
  }

  sockaddr_in address = { 0 };
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(receive_port_);
  memset(&(address.sin_zero), '\0', 8);

  if (bind(socket_fd_, reinterpret_cast<const sockaddr *>(&address),
           sizeof(address)) < 0) {
    return false;
  }

  return true;
}

std::string UdpPacketReader::read() {
  sockaddr_in sender_address = { 0 };
  socklen_t sender_address_len = sizeof(sender_address);

  while (true) {
    ssize_t result = recvfrom(socket_fd_, &receive_buffer_,
                              kReceiveBufferLength,
                              MSG_WAITALL,
                              reinterpret_cast<sockaddr *>(&sender_address),
                              &sender_address_len);
    if (result < 0) {
      continue;
    }

    if (sender_address.sin_addr.s_addr == my_address_.s_addr) {
      continue;
    }

    receive_buffer_[result] = '\0';
    return receive_buffer_;
  }
}

} // namespace airball