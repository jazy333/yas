#include "tfidf_scorer.h"

#include <cmath>

namespace yas {
TFIDFScorer::TFIDFScorer(float boost, float idf) : boost_(boost), idf_(idf) {}
float TFIDFScorer::score(long freq, long norm) {
  return sqrt(freq) * idf_ * boost_;
}
}  // namespace yas