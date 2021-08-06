#include "bitpacking_compression.h"

#include "common.h"

namespace yas {
BitPackingCompression::BitPackingCompression(int max_bit)
    : max_bits_(max_bit) {}

BitPackingCompression::~BitPackingCompression() {}

void BitPackingCompression::compress(const uint64_t* in, size_t in_size,
                                     uint8_t* out, size_t& out_size) {
  uint64_t* out64 = (uint64_t*)out;
  size_t out_size64 = out_size / sizeof(uint64_t);
  int most_sig_bits = 64 - max_bits_;
  uint64_t mask = ~0L << most_sig_bits;
  mask = mask >> most_sig_bits;

  for (size_t i = 0; i < in_size; ++i) {
    long total_bits = i * max_bits_;
    int block_index = total_bits / 64;
    int bit_index = total_bits % 64;

    if (bit_index <= most_sig_bits) {  // one block
      out64[block_index] &= (~(mask << bit_index)) | (in[i] << bit_index);
    } else {  // two blocks
      int end_bits = 64 - bit_index;
      uint64_t part_mask = ~(-1UL << bit_index);
      out64[block_index] &= (part_mask | (in[i] << bit_index));
      out64[block_index + 1] |= in[i] >> end_bits;
    }
  }
}

uint8_t* BitPackingCompression::decompress(const uint8_t* in, size_t in_size,
                                           uint64_t* out, size_t& out_size) {
  const uint64_t* in64 = static_cast<const uint64_t*>(in);
  size_t in_size64 = in_size / sizeof(uint64_t);
  int most_sig_bits = 64 - max_bits_;
  uint64_t mask = ~0L << most_sig_bits;
  mask = mask >> most_sig_bits;
  uint64_t* out_start = out;

  for (size_t i = 0; i < in_size64; ++i) {
    uint64_t total_bits = i * max_bits_;
    int block_index = total_bits / 64;
    int bit_index = total_bits % 64;
    uint64_t value;
    if (bit_index <= most_sig_bits) {  // one block
      value = ((in64[block_index] >> bit_index) & mask);
    } else {  // two blocks
      int end_bits = 64 - bit_index;
      value = (in64[block_index] >> bit_index |
              (in64[block_index + 1] <<end_bits);
    }
    *out++ = value;
  }
  out_size = out - out_start;
  return in + in_size;
}

}  // namespace yas