#include <cassert>
#include <cstring>
#include <vector>

#include "atomic_store.h"

namespace airball {

uint32_t kMagicNumber = 0xa13ba117;

uint8_t inverse_bank(uint8_t bank) {
  return bank == 0 ? 1 : 0;
}

struct AtomicStore::Header {
  uint32_t magic_number_0;
  uint64_t page_size;
  uint64_t bank_size;
  uint8_t bank;
  uint32_t magic_number_1;
};

AtomicStore::AtomicStore(const std::string &path) : path_(path) {}

void AtomicStore::initialize(size_t page_size, size_t bank_size) {
  Header h = {
      .magic_number_0 = kMagicNumber,
      .page_size = page_size,
      .bank_size = bank_size,
      .bank = 0,
      .magic_number_1 = kMagicNumber,
  };
  write_raw(0, (char *) &h, sizeof(h));
  std::vector<char> null_data(bank_size, 0);
  write_raw(bank_offset(&h, 0), null_data.data(), bank_size);
  write_raw(bank_offset(&h, 1), null_data.data(), bank_size);
}

 bool AtomicStore::is_initialized() {
  return is_correct(read_header().get());
}

size_t AtomicStore::page_size() {
  return read_correct_header()->page_size;
}

size_t AtomicStore::bank_size() {
  return read_correct_header()->bank_size;
}

uint8_t AtomicStore::bank() {
  return read_correct_header()->bank;
}

void AtomicStore::write_payload(const std::string& data) {
  std::shared_ptr<Header> h = read_correct_header();
  write_raw(bank_offset(h.get(), inverse_bank(h->bank)), data.c_str(), data.size());
  write_bank_number(inverse_bank(h->bank));
}

void AtomicStore::write_payload(uint8_t bank, const std::string& data) {
  assert(bank == 0 || bank == 1);
  std::shared_ptr<Header> h = read_correct_header();
  write_raw(bank_offset(h.get(), bank), data.c_str(), data.size());
  write_bank_number(bank);
}

std::string AtomicStore::read_payload() {
  std::shared_ptr<Header> h = read_correct_header();
  char data[h->bank_size];
  read_raw(bank_offset(h.get(), h->bank), data, h->bank_size);
  return data;
}

std::string AtomicStore::read_payload(uint8_t bank) {
  assert(bank == 0 || bank == 1);
  std::shared_ptr<Header> h = read_correct_header();
  char data[h->bank_size];
  read_raw(bank_offset(h.get(), bank), data, h->bank_size);
  return data;
}

std::shared_ptr<AtomicStore::Header> AtomicStore::read_correct_header() {
  auto header = read_header();
  assert(is_correct(header.get()));
  return header;
}

std::shared_ptr<AtomicStore::Header> AtomicStore::read_header() {
  auto header = std::make_shared<Header>();
  read_raw(0, (char *) header.get(), sizeof(Header));
  return header;
}

bool AtomicStore::is_correct(Header* header) {
  return
      header->magic_number_0 == kMagicNumber &&
      header->magic_number_1 == kMagicNumber;
}

size_t AtomicStore::bank_offset(Header* header, uint8_t bank) {
  size_t header_actual_size = ((sizeof(Header) / header->page_size) + 1) * header->page_size;
  size_t bank_actual_size = ((header->bank_size / header->page_size) + 1) * header->page_size;
  return header_actual_size + (bank * bank_actual_size);
}

void AtomicStore::write_raw(size_t offset, const char* data, size_t length) {
  FILE *f = fopen(path_.c_str(), "rb+");
  assert(f != nullptr);
  assert(fseek(f, offset, SEEK_SET) >= 0);
  assert(fwrite(data, sizeof(char), length, f) == length);
  assert(fsync(f->_fileno) >= 0);
  assert(fclose(f) >= 0);
}

void AtomicStore::read_raw(size_t offset, char* data, size_t length) {
  FILE *f = fopen(path_.c_str(), "rb+");
  assert(f != nullptr);
  assert(fseek(f, offset, SEEK_SET) >= 0);
  assert(fread(data, sizeof(char), length, f) == length);
  assert(fclose(f) >= 0);
}

void AtomicStore::write_bank_number(char bank) {
  write_raw(offsetof(Header, bank), &bank, 1);
}

};  // namespace airball