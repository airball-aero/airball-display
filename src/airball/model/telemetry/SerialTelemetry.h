#ifndef SRC_AIRBALL_MODEL_TELEMETRY_SERIALTELEMETRY_H
#define SRC_AIRBALL_MODEL_TELEMETRY_SERIALTELEMETRY_H

#include <string>
#include <thread>
#include <deque>
#include <mutex>
#include <condition_variable>

#include "ITelemetry.h"

#include "../../util/IDeviceFileAdapter.h"

namespace airball {

class SerialTelemetry : public ITelemetry {
public:
  SerialTelemetry(IDeviceFileAdapter* file);
  ~SerialTelemetry();

  Message receive() override;
  void send(Message m) override;
  void send(std::vector<Message> m) override;

private:
  IDeviceFileAdapter* file_;
  std::thread t_;

  std::mutex mu_;
  std::condition_variable readWait_;
  std::atomic<bool> running_;

  std::deque<char> incomingEncodedBytes_;
  std::deque<Message> outgoingMessages_;
};

} // namespace airball

#endif //SRC_AIRBALL_MODEL_TELEMETRY_SERIALTELEMETRY_H
