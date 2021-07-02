#pragma once
#include <cstdint>

namespace yas {
class BitSet {
 private:
  /* data */
 public:
  BitSet(/* args */);
  ~BitSet();
  virtual set(uint32_t) = 0;
  virtual bool get() = 0;
  virtual size_t size() = 0;
  virtual uint32_t next(uint32_t) = 0;
};
}  // namespace yas