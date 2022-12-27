#include "udp_packet_reader.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

namespace airball {

UdpPacketReader::UdpPacketReader(int receive_port)
    : receive_port_(receive_port), socket_fd_(0) {
  memset(receive_buffer_, 0, sizeof(receive_buffer_));
  open();
}

UdpPacketReader::~UdpPacketReader() {
  if (socket_fd_ != 0) {
    close(socket_fd_);
  }
}

bool UdpPacketReader::open() {
  socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_fd_ < 0) {
    return false;
  }

  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(receive_port_);

  if (bind(socket_fd_, reinterpret_cast<const sockaddr *>(&address),
           sizeof(address)) < 0) {
    return false;
  }

  return true;
}

std::string UdpPacketReader::readLine() {
  sockaddr_in sender_address{};
  socklen_t sender_address_len = sizeof(sender_address);
  ssize_t result = recvfrom(socket_fd_, &receive_buffer_,
                            kReceiveBufferLength,
                            MSG_WAITALL,
                            reinterpret_cast<sockaddr *>(&sender_address),
                            &sender_address_len);
  if (result < 0) {
    return "";
  }
  receive_buffer_[result] = '\0';
  return receive_buffer_;
}

} // namespace airball