#pragma once

#include <cstdint>

namespace yas {
class PostingList {
 public:
  virtual uint32_t next() = 0;
  virtual uint32_t advance(uint32_t target) = 0;
  virtual uint32_t docid() = 0;
  virtual long cost() = 0;
};
}  // namespace yas