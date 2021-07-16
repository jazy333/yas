#include "boolean_scorer.h"

namespace yas {
BooleanScorer::BooleanScorer(float boost): boost_(boost) {}
float BooleanScorer::score(long freq, long norm) { return boost_; }
}  // namespace yas