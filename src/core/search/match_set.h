#pragma once

#include "relevance.h"

namespace yas {
class MatchSet {
 public:
  MatchSet() = default;
  virtual MatchSet() = default;
  size_t size();
  void add(uint32_t did);

 private:
  Relevance* rel_;
};
}  // namespace yas