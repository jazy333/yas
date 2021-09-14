#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace yas {
class BitPackingCompression {
 public:
  BitPackingCompression(int max_bits, uint64_t min_value,uint64_t gcd);
  BitPackingCompression();
  virtual ~BitPackingCompression();
  void compress(const uint64_t* in, size_t in_size, uint8_t* out,
                size_t& out_size);
  uint8_t* decompress(const uint8_t* in, size_t in_size, uint64_t* out,
                      size_t& out_size);
  void set_max_bits(int max_bits);
  int get_max_bits();

  void set_min_value(uint64_t min_value);
  uint64_t get_min_value();

  void set_gcd(uint64_t gcd);
  uint64_t get_gcd();

 private:
  int max_bits_;
  uint64_t min_value_;
  uint64_t gcd_;
};
}  // namespace yas