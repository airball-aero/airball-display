#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include "SerialTelemetry.h"
#include "../../util/Base16Encoding.h"

const int kCount = 1024;

int main(int argc, char** argv) {
  std::string path = std::tmpnam(nullptr);
  if (mkfifo(path.c_str(), 0666) < 0) {
    std::cout << "FAIL mkfifo" << std::endl;
  }

  airball::SerialTelemetry st(path);

  std::thread remoteSenderThread([&path]() {
    int fd = open(path.c_str(), O_WRONLY);
    if (fd < 0) {
      std::cout << "FAIL open in remoteSenderThread" << std::endl;
    }
    for (uint16_t i = 0; i < kCount; i++) {
      airball::ITelemetry::Message m {
          .id = i,
          .data = { 0 },
      };
      std::string encoded = airball::Base16Encoding::encode((char *) &m, sizeof(m));
      encoded.push_back('\n');
      if (write(fd, encoded.data(), encoded.size()) != encoded.size()) {
        std::cout << "FAIL write in remoteSenderThread" << std::endl;
      }
    }
    close(fd);
  });

  std::string remoteReceivedEncoded;

  std::thread remoteReceiverThread([&path, &remoteReceivedEncoded]() {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
      std::cout << "FAIL open in remoteReceiverThread" << std::endl;
    }
    char c;
    int count = 0;
    while (count < kCount) {
      if (read(fd, &c, 1) != 1) {
        std::cout << "FAIL read in remoteReceiverThread" << std::endl;
      }
      remoteReceivedEncoded.push_back(c);
      if (c == '\n') {
        count++;
      }
    }
    close(fd);
  });

  std::cout << "started threads" << std::endl;

  for (uint16_t i = 0; i < kCount; i++) {
    st.send(airball::ITelemetry::Message {
        .id = static_cast<uint16_t>(i * 2),
        .data = { 0 },
    });
  }

  std::cout << "did the sends" << std::endl;

  std::vector<airball::ITelemetry::Message> localReceived;

  for (int i = 0; i < kCount; i++) {
    localReceived.push_back(st.receive());
  }

  std::cout << "did the receives" << std::endl;

  remoteSenderThread.join();
  remoteReceiverThread.join();


}