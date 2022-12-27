#ifndef AIRBALL_MODEL_TELEMETRY_I_LINE_READER_H
#define AIRBALL_MODEL_TELEMETRY_I_LINE_READER_H

#include <cstddef>
#include <string>

namespace airball {

class ILineReader {
public:
  virtual std::string readLine() = 0;
  virtual std::string foo() = 0;
};

} // namespace airball

#endif // AIRBALL_MODEL_TELEMETRY_I_LINE_READER_H
