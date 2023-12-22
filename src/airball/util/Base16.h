#ifndef SRC_AIRBALL_UTIL_BASE16_H
#define SRC_AIRBALL_UTIL_BASE16_H

#include <string>

namespace airball {

class Base16 {

  static std::string encode(std::string data);
  static std::string decode(std::string encoded);

};

} // namespace airball


#endif //SRC_AIRBALL_UTIL_BASE16_H
