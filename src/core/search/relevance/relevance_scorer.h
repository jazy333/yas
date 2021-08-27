#pragma once
#include "index_stat.h"
#include "term_stat.h"

namespace yas {
class RelevanceScorer {
 public:
 virtual ~RelevanceScorer()=default;
  virtual float score(long freq, long norm) = 0;
};
}  // namespace yas