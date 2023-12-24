#ifndef SRC_AIRBALL_UTIL_BASE16ENCODING_H
#define SRC_AIRBALL_UTIL_BASE16ENCODING_H

#include <string>

namespace airball {

class Base16Encoding {
public:
  static std::string encode(const char* decoded, size_t len);
  static std::string decode(const char* encoded, size_t len);
};

} // namespace airball

#endif //SRC_AIRBALL_UTIL_BASE16ENCODING_H
