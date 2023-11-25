#include <cstdlib>
#include <iostream>
#include <cstring>

#include "atomic_store.h"

constexpr char kTmpFileTemplate[] = "/tmp/atomic_store_XXXXXX";

#define ASSERT_TRUE(x) if (!(x)) { std::cout << "Assertion failed " << __FILE__ << ":" << __LINE__ << std::endl; }

int main(int arg, char** argv) {
  char tmp_file[128];
  strcpy(tmp_file, kTmpFileTemplate);
  if (mktemp(tmp_file) != tmp_file) {
    std::cerr << "Failed to make temp file" << std::endl;
    exit(-1);
  }

  fclose(fopen(tmp_file, "w"));

  airball::AtomicStore s(tmp_file);
  s.initialize(32, 128);

  ASSERT_TRUE(s.page_size() == 32);
  ASSERT_TRUE(s.bank_size() == 128);
  ASSERT_TRUE(s.bank() == 0);
  ASSERT_TRUE(s.read_payload() == "");
  ASSERT_TRUE(s.read_payload(0) == "");
  ASSERT_TRUE(s.read_payload(1) == "");

  s.write_payload("Hello world one");

  ASSERT_TRUE(s.bank() == 1);
  ASSERT_TRUE(s.read_payload() == "Hello world one");

  s.write_payload("Hello world zero");

  ASSERT_TRUE(s.bank() == 0);
  ASSERT_TRUE(s.read_payload() == "Hello world zero");

  ASSERT_TRUE(s.read_payload(0) == "Hello world zero");
  ASSERT_TRUE(s.read_payload(1) == "Hello world one");

  s.write_payload(0, "Hello world zero again");

  ASSERT_TRUE(s.bank() == 0);
  ASSERT_TRUE(s.read_payload() == "Hello world zero again");
  ASSERT_TRUE(s.read_payload(0) == "Hello world zero again");
  ASSERT_TRUE(s.read_payload(1) == "Hello world one");
}