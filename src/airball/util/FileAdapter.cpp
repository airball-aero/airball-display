#include "FileAdapter.h"
#include <fcntl.h>
#include <unistd.h>

namespace airball {

FileAdapter::FileAdapter(std::string path, int flags)
    : path_(path), flags_(flags), fd_(-1) {}

int FileAdapter::open() {
  fd_ = ::open(path_.c_str(), flags_);
  if (fd_ < 0) {
    return -1;
  }
}

int FileAdapter::close() {
  return ::close(fd_);
}

int FileAdapter::readWithTimeout(char* buf, size_t n, std::chrono::duration<int, std::micro> timeout) {
  fd_set set;
  FD_ZERO(&set);
  FD_SET(fd_, &set);

  struct timeval tm;
  tm.tv_sec = 0;
  tm.tv_usec = timeout.count();

  int rv = ::select(1, &set, NULL, NULL, &tm);

  if (rv <= 0) {
    return rv;
  } else {
    return read( fd_, buf, n);
  }
}

int FileAdapter::write(char* buf, size_t n) {
  return ::write(fd_, buf, n);
}

} // namespace airball
