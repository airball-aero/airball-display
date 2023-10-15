#include <iostream>
#include <sstream>
#include "UdpPacketReader.h"
#include "UdpPacketSender.h"

const int kReceivesPerSend = 10;

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <port> <interface> <bcast-ip>" << std::endl;
    return -1;
  }

  int port = atoi(argv[1]);
  std::string network_interface = argv[2];
  std::string broadcast_ip = argv[3];

  airball::UdpPacketReader r(port, network_interface);
  airball::UdpPacketSender s(broadcast_ip, port);

  int receive_count = 0;
  int send_count = 0;

  while (true) {
    for (int i = 0; i < kReceivesPerSend; i++) {
      std::cout << "[" << receive_count++ << "] Received: " << r.read() << std::endl;
    }

    std::stringstream ss;
    ss << "Udp message " << send_count++;
    s.send(ss.str());
  }
}
