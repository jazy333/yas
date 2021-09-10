#pragma once
#include "core/index/index_stat.h"
#include "core/search/term_stat.h"

namespace yas {
class RelevanceScorer {
 public:
 virtual ~RelevanceScorer()=default;
  virtual float score(long freq, long norm) = 0;
};
}  // namespace yas