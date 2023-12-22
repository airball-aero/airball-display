#ifndef AIRBALL_TELEMETRY_UDP_PACKET_READER_H
#define AIRBALL_TELEMETRY_UDP_PACKET_READER_H

#include <cstddef>
#include <chrono>
#include <string>
#include <netinet/in.h>

namespace airball {

class UdpPacketReader {
public:
  explicit UdpPacketReader(int receive_port, const std::string& receive_interface);
  ~UdpPacketReader();

  bool poll(std::chrono::duration<int, std::milli> timeout);

  std::string read();

private:
  bool open();

  const static size_t kReceiveBufferLength = 1024;

  const int receive_port_;
  const std::string receive_interface_;
  int socket_fd_;
  struct in_addr my_address_;
  char receive_buffer_[kReceiveBufferLength];
};

} // namespace airball


#endif // AIRBALL_TELEMETRY_UDP_PACKET_READER_H
