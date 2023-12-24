#ifndef AIRBALL_TELEMETRY_ITELEMETRY_H
#define AIRBALL_TELEMETRY_ITELEMETRY_H

#include <vector>

namespace airball {

class ITelemetry {
public:
  struct Message {
    uint16_t id;
    uint8_t data[8];
  };

  // Receive the next Message. Blocks until a Message is received.
  virtual Message receive() = 0;

  // Send a Message. This is broadcast to everyone on the network.
  virtual void send(Message s) = 0;

  // Send a series of Messages. This is broadcast to everyone on the network.
  // This method is merely an optimization, in case several Messages are ready
  // at once and can be bundled together.
  virtual void send(std::vector<Message> s) = 0;
};

}  // namespace airball

#endif // AIRBALL_TELEMETRY_ITELEMETRY_H
