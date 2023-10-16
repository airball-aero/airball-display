#include "NMEAFormat.h"

#include <vector>
#include <sstream>

namespace airball {

const auto kComma = std::string(",");
constexpr int kCommaLen = 1;

const std::string kAirdata = "$AR";
const std::string kSettingsRequest = "$SR";
const std::string kSettings = "$SS";

std::vector<std::string> split_comma(const std::string &s) {
  std::vector<std::string> tokens;
  std::string work(s);
  for (size_t pos = 0; (pos = work.find(kComma)) != std::string::npos;) {
    tokens.push_back(work.substr(0, pos));
    work.erase(0, pos + kCommaLen);
  }
  tokens.push_back(work);
  return tokens;
}

ITelemetry::Sample
parseAirdata(const std::string &message,
             const std::vector<std::string> &tokens) {
  return ITelemetry::Airdata {
      .sequence = (unsigned long) atoi(tokens[1].c_str()),
      .alpha = atof(tokens[2].c_str()),
      .beta = atof(tokens[3].c_str()),
      .q = atof(tokens[4].c_str()),
      .p = atof(tokens[5].c_str()),
      .t = atof(tokens[6].c_str()),
  };
}

ITelemetry::Sample
parseSettingsRequest(const std::string& message,
                     const std::vector<std::string> &tokens) {
  return ITelemetry::SettingsRequest {};
}

ITelemetry::Sample
parseSettings(const std::string& message,
              const std::vector<std::string> &tokens) {
  return ITelemetry::Settings {
    .value = message.substr(4),
  };
}

ITelemetry::Sample
NMEAFormat::unmarshal(const std::string &message) {
  auto tokens = split_comma(message);
  if (!tokens.empty()) {
    if (tokens[0] == kAirdata) {
      return parseAirdata(message, tokens);
    }
    if (tokens[0] == kSettingsRequest) {
      return parseSettingsRequest(message, tokens);
    }
    if (tokens[0] == kSettings) {
      return parseSettings(message, tokens);
    }
  }
  return ITelemetry::Unknown {};
}

std::string
marshalAirdata(ITelemetry::Airdata o) {
  std::stringstream s;
  s << kAirdata << ","
    << o.sequence << ","
    << o.alpha << ","
    << o.beta << ","
    << o.q << ","
    << o.p << ","
    << o.t;
  return s.str();
}

std::string
marshalSettingsRequest(ITelemetry::SettingsRequest o) {
  std::stringstream s;
  s << kSettingsRequest;
  return s.str();
}

std::string
marshalCompressedSettings(ITelemetry::Settings o) {
  std::stringstream s;
  s << kSettings << ","
    << o.value;
  return s.str();
}

std::string
NMEAFormat::marshal(ITelemetry::Sample s) {
  if (std::holds_alternative<ITelemetry::Airdata>(s)) {
    return marshalAirdata(std::get<ITelemetry::Airdata>(s));
  }
  if (std::holds_alternative<ITelemetry::SettingsRequest>(s)) {
    return marshalSettingsRequest(std::get<ITelemetry::SettingsRequest>(s));
  }
  if (std::holds_alternative<ITelemetry::Settings>(s)) {
    return marshalCompressedSettings(std::get<ITelemetry::Settings>(s));
  }
  return "";
}

}  // namespace airball
