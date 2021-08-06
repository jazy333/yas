#pragma once
#include <cstdint>
#include <vector>

namespace yas {
class BitPackingCompression {
 public:
  BitPackingCompression(int max_bits);
  virtual ~BitPackingCompression();
  void compress(const uint64_t* in, size_t in_size, uint64_t* out,
                size_t& out_size);
  uint8* decompress(const uint8_t* in, size_t in_size, uint8* out,
                      size_t& out_size);

 private:
  int max_bits_;
};
}  // namespace yas