#ifndef SRC_AIRBALL_MODEL_TELEMETRY_SERIALTELEMETRY_H
#define SRC_AIRBALL_MODEL_TELEMETRY_SERIALTELEMETRY_H

#include <string>
#include <thread>
#include <deque>
#include <mutex>
#include <condition_variable>

#include "ITelemetry.h"

namespace airball {

class SerialTelemetry : public ITelemetry {
public:
  SerialTelemetry(std::string path);

  Message receive() override;
  void send(Message m) override;
  void send(std::vector<Message> m) override;

private:
  std::string path_;
  std::thread t_;

  std::mutex mu_;
  std::condition_variable readWait_;

  std::deque<char> incomingEncodedBytes_;
  std::deque<Message> outgoingMessages_;
};

} // namespace airball

#endif //SRC_AIRBALL_MODEL_TELEMETRY_SERIALTELEMETRY_H
