#pragma once
#include "relevance.h"

namespace yas {
class BM25Relevance : public Relevance {
 public:
  BM25Relevance(float k1, float b);
  BM25Relevance();
  ~BM25Relevance();
  RelevanceScorer* scorer(float boost, IndexStat* index_stat,
                          TermStat* term_stat) override;

 protected:
  float idf(long doc_freq, long doc_count);

 private:
  void init_length(float avgdl);

 private:
  float k1_;
  float b_;
  static float part[256];
};
}  // namespace yas