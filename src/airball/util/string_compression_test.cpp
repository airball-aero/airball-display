#include "string_compression.h"

#include <iostream>
#include <sstream>

std::string stringToNums(const std::string& s) {
  std::stringstream ss;
  for (int i = 0; i < s.length() + 1; i++) {
    ss << (int) s[i] << " ";
  }
  return ss.str();
}

std::string testString() {
  std::stringstream ss;
  for (int i = 0; i < 128; i++) {
    ss << "Hello world from " << i << " ";
  }
  return ss.str();
}

int main(int argc, char* argv[]) {
  auto a = testString();
  std::cout << "a=" << a << std::endl;
  auto b = airball::compressString(a);
  std::cout << "b=" << stringToNums(b) << std::endl;
  auto c = airball::expandString(b);
  std::cout << "c=" << c << std::endl;
}
