#ifndef AIRBALL_UTIL_STRING_COMPRESSION_H
#define AIRBALL_UTIL_STRING_COMPRESSION_H

#include <string>

namespace airball {

std::string compressString(const std::string& s);
std::string expandString(const std::string& s);

}  // namespace airball

#endif  // AIRBALL_UTIL_STRING_COMPRESSION_H