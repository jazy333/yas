#pragma once
#include "bit_set.h"

namespace yas {
template <uint32_t N>
class FixedBitSet : public BitSet {
 private:
  uint32_t* bits_;
  size_t capcity_;

 public:
  FixedBitSet();
  ~FixedBitSet();
  virtual set(uint32_t) override;
  virtual bool get() override;
  virtual size_t size() override;
  size_t capcity() ;
  virtual uint32_t next(uint32_t) override;
  uint32_t* bits();
};

}  // namespace yas