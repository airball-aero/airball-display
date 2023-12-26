#include <iostream>
#include <cstring>
#include "SerialTelemetry.h"
#include "../../util/Base16Encoding.h"

const int kCount = 1024;
const int kChunkSizeIncludingNewline = (sizeof(airball::ITelemetry::Message) * 2) + 1;

class TestFileAdapter : public airball::IDeviceFileAdapter {
public:
  TestFileAdapter(std::string dataToSend)
      : dataToSend_(dataToSend), blockOnRead_(false) {
  }

  int open() override {
    return 0;
  }

  int close() override {
    return 0;
  }

  int readWithTimeout(char *buf, size_t n, std::chrono::duration<int, std::micro> timeout) override {
    if (blockOnRead_) {
      blockOnRead_ = !blockOnRead_;
      std::this_thread::sleep_for(timeout);
    }
    size_t k = std::min(std::min(n, (size_t) 7), dataToSend_.size());
    std::memcpy(buf, dataToSend_.data(), k);
    dataToSend_.erase(0, k);
    return k;
  }

  int write(char *buf, size_t n) override {
    dataWritten_.append(buf, n);
    return n;
  }

  const std::string& dataWritten() { return dataWritten_; }

private:
  std::atomic<bool> blockOnRead_;
  std::string dataToSend_;
  std::string dataWritten_;
};

int main(int argc, char** argv) {
  std::string dataToSend;

  for (int i = 0; i < kCount; i++) {
    airball::ITelemetry::Message m{
        .id = (uint16_t) i,
        .data = {0},
    };
    dataToSend.append(airball::Base16Encoding::encode((char *) &m, sizeof(m)));
    dataToSend.push_back('\n');
    if (i == kCount / 2) {
      for (int j = 0; j < 16; j++) {
        dataToSend.append("abcdefghijklmnopqrstuvwxyz\n");
        dataToSend.append("a\n");
        dataToSend.append("x\n");
      }
    }
  }

  TestFileAdapter adapter(dataToSend);
  airball::SerialTelemetry st(&adapter);

  std::thread sender([&st]() {
    for (uint16_t i = 0; i < kCount; i++) {
      st.send(airball::ITelemetry::Message{
          .id = static_cast<uint16_t>(i * 3),
          .data = {0},
      });
    }
  });

  std::vector<airball::ITelemetry::Message> received;

  std::thread receiver([&st, &received]() {
    for (int i = 0; i < kCount; i++) {
      received.push_back(st.receive());
    }
  });

  sender.join();
  receiver.join();

  if (received.size() != kCount) {
    std::cerr << "FAIL: received.size()" << std::endl;
  }
  for (int i = 0; i < kCount; i++) {
    if (received[i].id != i) {
      std::cerr << "FAIL: received[" << i << ".id" << std::endl;
    }
    for (int j = 0; j < sizeof(airball::ITelemetry::Message::data); j++) {
      if (received[i].data[j] != 0) {
        std::cerr << "FAIL: received[" << i << ".data[" << j << "]" << std::endl;
      }
    }
  }

  if (adapter.dataWritten().size() != kCount * kChunkSizeIncludingNewline) {
    std::cerr << "FAIL: dataWritten.size()" << std::endl;
  }

  for (int i = 0; i < kCount; i++) {
    std::string chunkIncludingNewline = adapter.dataWritten().substr(
        i * kChunkSizeIncludingNewline,
        kChunkSizeIncludingNewline);
    char buf[5];
    uint16_t n = i * 3;
    snprintf(buf, 5, "%02x%02x", n & 0x00ff, (n >> 8));
    if (chunkIncludingNewline.substr(0, 4) != buf) {
      std::cerr << "FAIL: dataWritten chunk " << i << " id" << std::endl;
    }
    if (chunkIncludingNewline.substr(4, kChunkSizeIncludingNewline - 5) != "0000000000000000") {
      std::cerr << "FAIL: dataWritten chunk " << i << " data" << std::endl;
    }
    if (chunkIncludingNewline[kChunkSizeIncludingNewline - 1] != '\n') {
      std::cerr << "FAIL: dataWritten chunk " << i << std::endl;
    }
  }
}