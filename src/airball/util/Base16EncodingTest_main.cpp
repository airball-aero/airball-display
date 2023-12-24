#include <string>
#include <iostream>

#include "Base16Encoding.h"

void testRoundTrip() {
  std::string decoded;
  for (int i = 0; i < 0xff; i++) {
    decoded.push_back(i);
  }
  std::string encoded = airball::Base16Encoding::encode(decoded);
  std::string newly_decoded = airball::Base16Encoding::decode(encoded);
  if (decoded != newly_decoded) {
    std::cout << "FAIL, decode round trip" << std::endl;
  }
}

void testKnownEncoding() {
  std::string decoded("abcdef");
  std::string encoded = airball::Base16Encoding::encode(decoded);
  if (encoded != "616263646566") {
    std::cout << "FAIL, encode known " << encoded << std::endl;
  }
}

void testKnownDecoding() {
  std::string encoded("616263646566");
  std::string decoded = airball::Base16Encoding::decode(encoded);
  if (decoded != "abcdef") {
    std::cout << "FAIL, decode known " << decoded << std::endl;
  }
}

int main(int arg, char** argv) {
  testRoundTrip();
  testKnownEncoding();
  testKnownDecoding();
}