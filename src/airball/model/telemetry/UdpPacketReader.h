#ifndef AIRBALL_TELEMETRY_UDP_PACKET_READER_H
#define AIRBALL_TELEMETRY_UDP_PACKET_READER_H

#include <cstddef>
#include <string>

namespace airball {

class UdpPacketReader {
public:
  explicit UdpPacketReader(int receive_port);
  ~UdpPacketReader();

  std::string readLine();

private:
  bool open();

  const static size_t kReceiveBufferLength = 1024;

  const int receive_port_;
  int socket_fd_;
  char receive_buffer_[kReceiveBufferLength];
};

} // namespace airball


#endif // AIRBALL_TELEMETRY_UDP_PACKET_READER_H
