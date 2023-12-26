#ifndef SRC_AIRBALL_UTIL_IDEVICEFILEADAPTER_H
#define SRC_AIRBALL_UTIL_IDEVICEFILEADAPTER_H

#include <chrono>

namespace airball {

class IDeviceFileAdapter {
public:
  virtual int open() = 0;
  virtual int close() = 0;
  virtual int readWithTimeout(char* buf, size_t n, std::chrono::duration<int, std::micro> timeout) = 0;
  virtual int write(char* buf, size_t n) = 0;
};

} // namespace airball

#endif //SRC_AIRBALL_UTIL_IDEVICEFILEADAPTER_H