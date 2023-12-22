#ifndef AIRBALL_TELEMETRY_ITELEMETRY_H
#define AIRBALL_TELEMETRY_ITELEMETRY_H

#include <string>
#include <variant>

namespace airball {

class ITelemetry {
public:
  struct Message {
    uint16_t id;
    uint8_t data[8];
  };

  // Receive the next Message. Blocks until a Message is received.
  virtual Message receiveMessage() = 0;

  // Send a Message. This is broadcast to everyone on the network.
  virtual void sendMessage(Message s) = 0;
};

}  // namespace airball

#endif // AIRBALL_TELEMETRY_ITELEMETRY_H
