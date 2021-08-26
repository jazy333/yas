#pragma once
#include "relevance.h"

namespace yas {
class TFIDFRelevance : public Relevance {
 public:
  TFIDFRelevance() = default;
  virtual ~TFIDFRelevance() = default;
  RelevanceScorer* scorer(float boost, IndexStat index_stat,
                          TermStat term_stat) override;

 private:
  float idf(long doc_freq, long doc_count);
};

}  // namespace yas