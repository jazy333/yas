#include "bitpacking_compression.h"

#include <cstring>

#include "common.h"

namespace yas {
BitPackingCompression::BitPackingCompression(int max_bit, uint64_t min_value)
    : max_bits_(max_bit), min_value_(min_value) {}

BitPackingCompression::BitPackingCompression() : max_bits_(-1), min_value_(0) {}

BitPackingCompression::~BitPackingCompression() {}

void BitPackingCompression::compress(const uint64_t* in, size_t in_size,
                                     uint8_t* out, size_t& out_size) {
  memset(out, 0, out_size);
  uint64_t* out64 = (uint64_t*)out;
  size_t out_size64 = out_size / sizeof(uint64_t);
  int most_sig_bits = 64 - max_bits_;
  uint64_t mask = ~0L << most_sig_bits;
  mask = mask >> most_sig_bits;

  int block_index = 0;
  for (size_t i = 0; i < in_size; ++i) {
    long total_bits = i * max_bits_;
    block_index = total_bits / 64;
    int bit_index = total_bits % 64;

    uint64_t value = in[i] - min_value_;

    if (bit_index <= most_sig_bits) {  // one block
      // out64[block_index] &= ~(mask << bit_index);  // clear bits
      out64[block_index] |= (value & mask) << bit_index;
    } else {  // two blocks
      int end_bits = 64 - bit_index;
      uint64_t part_mask = ~(-1UL << bit_index);
      // out64[block_index] &= part_mask;
      out64[block_index] |= value << bit_index;
      // out64[++block_index]&=~(-1ul>>max_bits_-endbits);
      out64[++block_index] |= value >> end_bits;
    }
  }
  out_size = (block_index + 1) * sizeof(uint64_t);
}

uint8_t* BitPackingCompression::decompress(const uint8_t* in, size_t in_size,
                                           uint64_t* out, size_t& out_size) {
  const uint64_t* in64 = reinterpret_cast<const uint64_t*>(in);
  int most_sig_bits = 64 - max_bits_;
  uint64_t mask = ~0L << most_sig_bits;
  mask = mask >> most_sig_bits;
  uint64_t* out_start = out;

  for (size_t i = 0; i < in_size; ++i) {
    uint64_t total_bits = i * max_bits_;
    int block_index = total_bits / 64;
    int bit_index = total_bits % 64;
    uint64_t value;
    if (bit_index <= most_sig_bits) {  // one block
      value = ((in64[block_index] >> bit_index) & mask);
    } else {  // two blocks
      int end_bits = 64 - bit_index;
      value = ((in64[block_index] >> bit_index) |
               (in64[block_index + 1] << end_bits)) &
              mask;
    }
    *out++ = (value + min_value_);
  }
  out_size = out - out_start;
  return const_cast<uint8_t*>(in + in_size);
}

void BitPackingCompression::set_max_bits(int max_bits) { max_bits_ = max_bits; }

int BitPackingCompression::get_max_bits() { return max_bits_; }

void BitPackingCompression::set_min_value(uint64_t min_value) {
  min_value_ = min_value;
}

uint64_t BitPackingCompression::get_min_value() { return min_value_; }

}  // namespace yas