#pragma once
#include "core/index/index_stat.h"
#include "relevance_scorer.h"
#include "core/search/term_stat.h"

namespace yas {
class Relevance {
 public:
  virtual ~Relevance() = default;
  virtual RelevanceScorer* scorer(float boost, IndexStat index_stat,
                                  TermStat term_stat) = 0;
};
}  // namespace yas