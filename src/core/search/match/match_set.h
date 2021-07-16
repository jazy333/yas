#pragma once

#include "matched_doc.h"

namespace yas {
class MatchSet {
 public:
  MatchSet() = default;
  virtual ~MatchSet() = default;
  size_t size();
  void add(const MatchedDoc& doc);
};
}  // namespace yas