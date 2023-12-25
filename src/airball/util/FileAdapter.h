#ifndef SRC_AIRBALL_UTIL_FILEADAPTER_H
#define SRC_AIRBALL_UTIL_FILEADAPTER_H

#include <string>

#include "IFileAdapter.h"

namespace airball {

class FileAdapter : public IFileAdapter {
public:
  FileAdapter(std::string path, int flags);

  int open() override;
  int close() override;
  int readWithTimeout(char* buf, size_t n, std::chrono::duration<int, std::micro> timeout) override;
  int write(char* buf, size_t n) override;

private:
  std::string path_;
  int flags_;
  int fd_;
};

} // namespace airball

#endif //SRC_AIRBALL_UTIL_FILEADAPTER_H
