#include "SerialTelemetry.h"

#include <fcntl.h>
#include <unistd.h>
#include <optional>
#include <iostream>

#include "../../util/Base16Encoding.h"

namespace airball {

auto kReopenTimeout = std::chrono::milliseconds(50);
auto kReadTimeoutMicros = std::chrono::microseconds (500);
size_t kReadBufSize = 64;

int readWithTimeout(int fd, char* buf, int n) {
  fd_set set;
  FD_ZERO(&set);
  FD_SET(fd, &set);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = kReadTimeoutMicros.count();

  std::cout << "select" << std::endl;
  int rv = select(1, &set, NULL, NULL, &timeout);
  std::cout << "done select" << std::endl;

  if (rv <= 0) {
    std::cout << "nada" << std::endl;
    return rv;
  } else {
    int c = read( fd, buf, n);
    std::cout << "read " << c << std::endl;
  }
}

SerialTelemetry::SerialTelemetry(std::string path)
    : path_(path) {
  t_ = std::thread([this]() {
    std::string readBuf(kReadBufSize, 0);

    while (true) {
      int fd = open(path_.c_str(), O_RDWR, 0);
      if (fd < 0) {
        std::this_thread::sleep_for(kReopenTimeout);
        continue;
      }

      while (true) {

        {
          int n = readWithTimeout(fd, readBuf.data(), readBuf.length());
          if (n < 0) {
            std::cout << "read err" << std::endl;
            break;
          } else if (n > 0) {
            std::unique_lock<std::mutex> lock(mu_);
            for (int i = 0; i < n; i++) {
              incomingEncodedBytes_.push_back(readBuf.at(i));
            }
            std::cout << "readWait_.notify_one()" << std::endl;
            readWait_.notify_one();
          }
        }

        bool writeError = false;

        while (true) {
          std::optional<Message> nextOutgoing;
          {
            std::unique_lock<std::mutex> lock(mu_);
            if (!outgoingMessages_.empty()) {
              nextOutgoing = outgoingMessages_.front();
            }
          }
          if (!nextOutgoing.has_value()) {
            break;
          }
          std::string outgoingData = Base16Encoding::encode(
              (const char*) &(nextOutgoing.value()),
              sizeof(Message));
          outgoingData.push_back('\n');
          int n = write(fd, outgoingData.data(), outgoingData.size());
          if (n == outgoingData.size()) {
            std::unique_lock<std::mutex> lock(mu_);
            outgoingMessages_.pop_front();
          } else {
            writeError = true;
            break;
          }
        }

        if (writeError) {
          break;
        }
      }

      close(fd);
    }
  });
}

ITelemetry::Message SerialTelemetry::receive() {
  std::string incomingEncodedChunk;
  while (true) {
    bool chunk = false;
    std::cout << "while" << std::endl;
    {
      std::unique_lock<std::mutex> lock(mu_);
      std::cout << "readWait_.wait" << std::endl;
      readWait_.wait(lock, [this]() { return !incomingEncodedBytes_.empty(); });
      std::cout << "readWait_ done wait" << std::endl;
      while (!incomingEncodedBytes_.empty()) {
        char c = incomingEncodedBytes_.front();
        incomingEncodedBytes_.pop_front();
        if (c == '\n') {
          chunk = true;
          break;
        } else {
          incomingEncodedChunk.push_back(c);
        }
      }
    }
    if (chunk) {
      std::string incomingDecodedChunk = Base16Encoding::decode(
          incomingEncodedChunk.data(),
          incomingEncodedChunk.size());
      if (incomingDecodedChunk.size() == sizeof(Message)) {
        return *((Message *) incomingDecodedChunk.data());
      }
    }
  }
}

void SerialTelemetry::send(Message m) {
  std::unique_lock<std::mutex> lock(mu_);
  outgoingMessages_.push_back(m);
}

void SerialTelemetry::send(std::vector<Message> m) {
  for (auto i : m) {
    std::unique_lock<std::mutex> lock(mu_);
    outgoingMessages_.push_back(i);
  }
}

} // namespace airball