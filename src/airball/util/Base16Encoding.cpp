#include "Base16Encoding.h"

#include <unordered_map>

namespace airball {

typedef std::unordered_map<u_int8_t, u_int8_t> CharToCharMap;

uint8_t kAsciiChar_0 = 0x30;
uint8_t kAsciiChar_a = 0x61;

std::pair<CharToCharMap, CharToCharMap> makeMaps() {
  CharToCharMap nyblToChar;
  CharToCharMap charToNybl;
  for (int i = 0; i < 0x0a; i++) {
    nyblToChar.insert({i, kAsciiChar_0 + i});
    charToNybl.insert({kAsciiChar_0 + i, i});
  }
  for (int i = 0x0a; i < 0x10; i++) {
    nyblToChar.insert({i, kAsciiChar_a + i - 0x0a});
    charToNybl.insert({ kAsciiChar_a + i - 0x0a, i});
  }
  return { nyblToChar, charToNybl };
}

std::pair<CharToCharMap, CharToCharMap> maps = makeMaps();

const CharToCharMap kNyblToChar = maps.first;
const CharToCharMap kCharToNybl = maps.second;

uint8_t nybl1(uint8_t byte) {
  return byte & 0x0f;
}

uint8_t nybl0(uint8_t byte) {
  return (byte >> 4) & 0x0f;
}

uint8_t byte(uint8_t nybl0, uint8_t nybl1) {
  return (nybl0 << 4) + nybl1;
}

std::string Base16Encoding::encode(const char* decoded, size_t len) {
  std::string encoded(len * 2, 0);
  for (int i = 0; i < len; i++) {
    encoded[2 * i] = kNyblToChar.at(nybl0(decoded[i]));
    encoded[2 * i + 1] = kNyblToChar.at(nybl1(decoded[i]));
  }
  return encoded;
}

std::string Base16Encoding::decode(const char* encoded, size_t len) {
  std::string decoded(len / 2, 0);
  for (int i = 0; i < len; i++) {
    decoded[i] = byte(
        kCharToNybl.at(encoded[2 * i]),
        kCharToNybl.at(encoded[2 * i + 1]));
  }
  return decoded;
}

} // namespace airball