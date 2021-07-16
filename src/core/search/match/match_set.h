#pragma once

#include "relevance.h"
#include "matched_doc.h"

namespace yas {
class MatchSet {
 public:
  MatchSet() = default;
  virtual ~MatchSet() = default;
  size_t size();
  void add(const MatchedDoc& doc);

 private:
  Relevance* rel_;
};
}  // namespace yas