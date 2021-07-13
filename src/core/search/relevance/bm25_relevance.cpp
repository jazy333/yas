#include "bm25_relevance.h"

#include <cmath>

namespace yas {
BM25Relevance::BM25Relevance(float k1, float b) : k1_(k1), b_(b) {}
BM25Relevance::BM25Relevance() k1_(1.2f), b_(0.75f) {}
BM25Relevance::~BM25Relevance() {}
/*  log(1 + (docCount - docFreq + 0.5)/(docFreq +0.5))
 */
float BM25Relevance::idf(long doc_freq, long doc_count) {
  return (float)log(1 + (doc_count - doc_freq + 0.5) / (doc_freq + 0.5));
}
float BM25Relevance::score(float freq, long norm) { return 0.0; }
}