#include "bm25_relevance.h"

#include <cmath>
#include <cstdint>
#include <mutex>

#include "bm25_scorer.h"
#include "common.h"

namespace yas {
float BM25Relevance::part[256];

void BM25Relevance::init_length(float avgdl) {
  for (uint16_t i = 0; i <= 255; ++i) {
    part[i] = 1.0 / (k1_ * ((1 - b_) + b_ * (uchar2uint(i) / avgdl)));
  }
}

BM25Relevance::BM25Relevance(float k1, float b) : k1_(k1), b_(b) {}

BM25Relevance::BM25Relevance() : k1_(1.2f), b_(0.75f) {}

BM25Relevance::~BM25Relevance() {}

/*  log(1 + (docCount - docFreq + 0.5)/(docFreq +0.5))
 */
float BM25Relevance::idf(long doc_freq, long doc_count) {
  return (float)log(1 + (doc_count - doc_freq + 0.5) / (doc_freq + 0.5));
}

RelevanceScorer* BM25Relevance::scorer(float boost, IndexStat index_stat,
                                       TermStat term_stat) {
  float weight = idf(term_stat.get_doc_freq(), index_stat.doc_count);
  float avgdl = 1.0;
  if (index_stat.doc_count != 0 && index_stat.total_term_freq != 0)
    avgdl = index_stat.total_term_freq / index_stat.doc_count;
  init_length(avgdl);
  return new BM25Scorer(weight, part);
}
}  // namespace yas