#ifndef AIRBALL_UTIL_ATOMIC_STORE_H
#define AIRBALL_UTIL_ATOMIC_STORE_H

#include <string>
#include <memory>

namespace  airball {

class AtomicStore {
public:
  AtomicStore(const std::string& path);

  void initialize(size_t page_size, size_t bank_size);
  bool is_initialized();

  size_t page_size();
  size_t bank_size();
  uint8_t bank();

  void write_payload(const std::string& data);
  void write_payload(uint8_t bank, const std::string& data);

  std::string read_payload();
  std::string read_payload(uint8_t bank);

private:
  struct Header;

  std::shared_ptr<Header> read_correct_header();
  std::shared_ptr<Header> read_header();
  bool is_correct(Header* header);

  size_t bank_offset(Header* header, uint8_t bank);
  void write_raw(size_t offset, const char* data, size_t length);
  void read_raw(size_t offset, char* data, size_t length);
  void write_bank_number(char bank);

  const std::string path_;
};

};

#endif  // AIRBALL_UTIL_ATOMIC_STORE_H

