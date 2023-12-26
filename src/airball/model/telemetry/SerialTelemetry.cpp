#include "SerialTelemetry.h"

#include <unistd.h>
#include <optional>

#include "../../util/Base16Encoding.h"

namespace airball {

auto kReopenTimeout = std::chrono::milliseconds(50);
auto kReadTimeoutMicros = std::chrono::microseconds (500);
size_t kReadBufSize = 64;

SerialTelemetry::SerialTelemetry(IDeviceFileAdapter* file)
    : file_(file), running_(true) {
  t_ = std::thread([this]() {
    std::string readBuf(kReadBufSize, 0);

    while (running_) {
      int rc = file_->open();
      if (rc < 0) {
        std::this_thread::sleep_for(kReopenTimeout);
        continue;
      }

      while (running_) {
        {
          int n = file_->readWithTimeout(readBuf.data(), readBuf.length(), kReadTimeoutMicros);
          if (n < 0) {
            break;
          } else if (n > 0) {
            std::unique_lock<std::mutex> lock(mu_);
            for (int i = 0; i < n; i++) {
              incomingEncodedBytes_.push_back(readBuf.at(i));
            }
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
          int n = file_->write(outgoingData.data(), outgoingData.size());
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

      file_->close();
    }
  });
}

SerialTelemetry::~SerialTelemetry() {
  running_ = false;
  t_.join();
}

ITelemetry::Message SerialTelemetry::receive() {
  std::string incomingEncodedChunk;
  while (true) {
    bool chunk = false;
    {
      std::unique_lock<std::mutex> lock(mu_);
      readWait_.wait(lock, [this]() { return !incomingEncodedBytes_.empty(); });
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
      try {
        std::string incomingDecodedChunk = Base16Encoding::decode(
            incomingEncodedChunk.data(),
            incomingEncodedChunk.size());
        if (incomingDecodedChunk.size() == sizeof(Message)) {
          return *((Message *) incomingDecodedChunk.data());
        }
      } catch (std::exception &e) {
        incomingEncodedChunk = "";
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