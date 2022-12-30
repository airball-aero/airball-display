#include <iostream>
#include "UdpPacketReader.h"

int main(int argc, char** argv) {
  airball::UdpPacketReader r(30123);
  while (true) {
    std::cout << r.readLine() << std::endl << std::flush;
  }
}
