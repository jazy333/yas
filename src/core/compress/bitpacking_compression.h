#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace yas {
class BitPackingCompression {
 public:
  BitPackingCompression(int max_bits);
   BitPackingCompression();
  virtual ~BitPackingCompression();
  void compress(const uint64_t* in, size_t in_size, uint8_t* out,
                size_t& out_size);
  uint8_t* decompress(const uint8_t* in, size_t in_size, uint64_t* out,
                      size_t& out_size);
  void set_max_bits(int max_bits);
  int get_max_bits();

 private:
  int max_bits_;
};
}  // namespace yas