#ifndef SRC_AIRBALL_MODEL_TELEMETRY_MESSAGEBLOCKFORMAT_H
#define SRC_AIRBALL_MODEL_TELEMETRY_MESSAGEBLOCKFORMAT_H

#include <string>
#include <vector>

#include "ITelemetry.h"

namespace airball {

class MessageBlockFormat {
public:
  static std::string marshal(std::vector<ITelemetry::Message> m);
  static std::vector<ITelemetry::Message> unmarshal(std::string block);
};

} // namespace airball

#endif // SRC_AIRBALL_MODEL_TELEMETRY_MESSAGEBLOCKFORMAT_H
