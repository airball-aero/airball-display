#include "telemetry.h"

#include <vector>

const auto kComma = std::string(",");
constexpr int kCommaLen = 1;

namespace airball {

std::vector<std::string> split_comma(const std::string& s) {
  std::vector<std::string> tokens;
  std::string work(s);
  for (size_t pos = 0; (pos = work.find(kComma)) != std::string::npos; ) {
    tokens.push_back(work.substr(0, pos));
    work.erase(0, pos + kCommaLen);
  }
  tokens.push_back(work);
  return tokens;
}

Telemetry::Sample
parse_airdata(const std::vector<std::string>& tokens) {
  return {
    .type = Telemetry::AIRDATA,
    .sequence = atoi(tokens[1].c_str()),
    .airdata = {
        .alpha = atof(tokens[2].c_str()),
        .beta = atof(tokens[3].c_str()),
        .q = atof(tokens[4].c_str()),
        .p = atof(tokens[5].c_str()),
        .t = atof(tokens[6].c_str()),
    }
  };
}

Telemetry::Sample
Telemetry::parse(const std::string& message) {
  auto tokens = split_comma(message);
  if (tokens[0] == "$AR") {
    return parse_airdata(tokens);
  }
  return {
    .type = UNKNOWN,
    .sequence = 0,
  };
}

}  // namespace airball