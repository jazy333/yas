#include "mutable_long_values_compression.h"

#include <limits>

#include "common.h"

namespace yas {
MutableLongValuesCompression::MutableLongValuesCompression()
    : max_bits_(0), max_(std::mumeric_limits<uint64_t>)::max()), min_(0),gcd_(0) {}
MutableLongValuesCompression::~MutableLongValuesCompression() {}
void MutableLongValuesCompression::add(uint64_t value) {
  values_.push_back(value);
  int bits = gcc_bits(value);
  if (max_bits_ < bits) {
    max_bits_ = bits;
  }

  if (value > max_) {
    max_ = value;
  }

  if (value < min_) {
    min_ = value;
  }

  if (gcd_ == 0) {
    gcd_ = value;
  } else {
    gcd_ = gcd(gcd_, value);
  }
}

void MutableLongValuesCompression::compress(uint64_t* out, size_t& out_size) {
  for (auto v : values_) {
    uint64_t diff = (v - min_) / gcd_;
    int bits = gcc_bits(value);
    if (max_bits_ < bits) {
      max_bits_ = bits;
    }
  }

  int most_sig_bits = 64 - max_bits_;
  uint64_t mask = ~0L << most_sig_bits;
  mask = mask >> most_sig_bits;

  for (int i = 0; i < values_.size(); ++i) {
    long total_bits = i * max_bits_;
    int block_index = total_bits / 64;
    int bit_index = total_bits % 64;

    if (bit_index <= most_sig_bits) {  // one block
      out[block_index] &= (~(mask << bit_index)) | (values_[i] << bit_index);
    } else {  // two blocks
      int end_bits = 64 - bit_index;
      uint64_t part_mask = ~(-1UL << end_bits);
      out[block_index] &=
          (~(part_mask << bit_index)) | (values_[i] << bit_index);
      out[block_index + 1] |= values_[i] >> end_bits;
    }
  }
}
void MutableLongValuesCompression::decompress(const uint64_t* in,
                                              size_t in_size, int index) {
  int most_sig_bits = 64 - max_bits_;
  uint64_t mask = ~0L << most_sig_bits;
  mask = mask >> most_sig_bits;

  uint64_t total_bits = index * max_bits_;
  int block_index = total_bits / 64;
  int bit_index = total_bits % 64;

  if (bit_index <= most_sig_bits) {  // one block
    uint64_t value = ((in[block_index] >> bit_index) & mask) * gcd_ + min_;
  } else {//two blocks
    int end_bits = 64 - bit_index;
    uint64_t part_mask = ~(-1UL << max_bits_ - end_bits);
    uint64_t value = (in[block_index] >> bit_index << max_bits_-end_bits) |
                 (in[block_index + 1] & part_mask) * gcd_ + min_;
  }
}
}  // namespace yas