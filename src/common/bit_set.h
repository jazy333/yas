#pragma once
#include <cstdint>

namespace yas {
class BitSet {
 public:
  virtual void set(uint64_t target) = 0;
  virtual bool get(uint64_t target) = 0;
  virtual size_t size() = 0;
  virtual size_t capcity() = 0;
  virtual uint64_t next(uint64_t target) = 0;
  virtual uint64_t* bits()=0;
};
}  // namespace yas