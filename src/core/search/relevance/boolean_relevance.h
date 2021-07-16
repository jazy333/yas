#pragma once
#include "relevance.h"
#include "relevance_scorer.h"

namespace yas {
class BooleanRelevance : public Relevance {
 public:
  BooleanRelevance() = default;
  ~BooleanRelevance() = default;
  RelevanceScorer* scorer(float boost, IndexStat* index_stat,
                          TermStat* term_stat) override;
};
}  // namespace yas