#include "MessageBlockFormat.h"

namespace airball {

std::string MessageBlockFormat::marshal(std::vector<ITelemetry::Message> m) {
  std::string bytes(sizeof(uint32_t) + m.size() * sizeof(ITelemetry::Message), 0);
  *((uint32_t*) bytes.data()) = m.size();
  for (int i = 0; i < m.size(); i++) {
    char* p = bytes.data() + sizeof(uint32_t) + i * sizeof(ITelemetry::Message);
    *((ITelemetry::Message *) p) = m[i];
  }
  return bytes;
}

std::vector<ITelemetry::Message> MessageBlockFormat::unmarshal(std::string block) {
  std::vector<ITelemetry::Message> r;
  uint32_t size = *((uint32_t*) block.data());
  for (int i = 0; i < size; i++) {
    char* p = block.data() + sizeof(uint32_t) + i * sizeof(ITelemetry::Message);
    r.push_back(*((ITelemetry::Message *) p));
  }
  return r;
}

} // namespace airball