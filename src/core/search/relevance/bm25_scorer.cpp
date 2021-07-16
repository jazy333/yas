#include "bm25_scorer.h"

namespace yas {

BM25Scorer::BM25Scorer(float term_weight, float* norm_table)
    : term_weight_(term_weight), norm_table_(norm_table) {}

/*
 *the obtained bm25 weight by the constant (k1+ 1) in order to
 *normalize the weight of terms with a frequency equals to 1
 *that occurs in documents with an average length
 */

float BM25Scorer::score(long freq, long norm) {
  float invert_norm = norm_table_[norm & 0xff];
  return term_weight_ - term_weight_ / (1.0f + freq / invert_norm);
}
}  // namespace yas