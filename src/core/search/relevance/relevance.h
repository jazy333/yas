#pragma once
#include "relevance_scorer.h"

namespace yas {
class Relevance {
 public:
  virtual RelevanceScorer* scorer(float boost,IndexStat* index_stat,
                                  TermStat* term_stat) = 0;
};
}  // namespace yas