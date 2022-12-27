#ifndef AIRBALL_TELEMETRY_UDP_PACKET_READER_H
#define AIRBALL_TELEMETRY_UDP_PACKET_READER_H

#include <cstddef>
#include <string>
#include "ILineReader.h"

namespace airball {

class UdpPacketReader : public ILineReader {
public:
  explicit UdpPacketReader(int receive_port);
  ~UdpPacketReader();

  std::string readLine() override;

  std::string foo() override { return "foo"; }

private:
  bool open();

  const static size_t kReceiveBufferLength = 1024;

  const int receive_port_;
  int socket_fd_;
  char receive_buffer_[kReceiveBufferLength];
};

} // namespace airball


#endif // AIRBALL_TELEMETRY_UDP_PACKET_READER_H
