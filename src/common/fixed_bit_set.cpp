#include "fixed_bit_set.h"

namespace yas {
FixedBitSet::FixedBitSet() { bits_ = new uint32_t[round(N, 4) / 4]; }

FixedBitSet::~FixedBitSet() { delete[] bits_; }

void FixedBitSet::set(uint32_t) {
  int index = target / N;
  int offset = target / (sizeof(uint32_t) * 4);
  bits_[index] |= 1 << offset;
}

bool FixedBitSet::get(uint32_t target) {
  int index = target / N;
  int offset = target / (sizeof(uint32_t) * 4);
  if (1 << offset & bits_[index]) {
    return true;
  } else {
    return false;
  }
}

size_t FixedBitSet::size() { return N; }

uint32_t FixedBitSet::next(uint32_t target) {
  int index = target / N;
  int offset = target % N;
  uint32_t word = bits_[index] >> offset;

  while (index < N) {
    word = bits_[index];
    if (word != 0) {
      return index << 5 + __builtin_clz(word);
    }
  }

  return -1;
}
}  // namespace yas