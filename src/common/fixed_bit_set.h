#pragma once
#include <cstddef>
#include <cstdint>

#include "bit_set.h"
#include "common.h"

namespace yas {
class FixedBitSet : public BitSet {
 public:
  FixedBitSet(uint64_t N) :N_(N){
    capcity_ = round(N, kNBits) / kNBits;
    bits_ = new uint64_t[capcity_]();
  }

  virtual ~FixedBitSet() { delete[] bits_; }

  void set(uint64_t target) override {
    int index = target / capcity_;
    int offset = target % kNBits;
    bits_[index] |= 1 << offset;
  }

  bool get(uint64_t target) {
    int index = target / capcity_;
    int offset = target % kNBits;
    if (1 << offset & bits_[index]) {
      return true;
    } else {
      return false;
    }
  }

  size_t size() override { return N_; }

  size_t capcity() override { return capcity_; }

  uint64_t next(uint64_t target) override {
    int index = target / capcity_;
    int offset = target % kNBits;
    uint64_t word = bits_[index] >> offset;

    if (word != 0) {
      return target + __builtin_ctzl(word);
    }

    while (++index < capcity_) {
      word = bits_[index];
      if (word != 0) {
        return index * kNBits + __builtin_ctzl(word);
      }
    }

    return -1;
  }

  uint64_t* bits() override { return bits_; }

 private:
  uint64_t* bits_;
  size_t capcity_;
  static const int kNBits;
  uint64_t N_;
};
const int FixedBitSet::kNBits = sizeof(uint64_t) * 8;

}  // namespace yas