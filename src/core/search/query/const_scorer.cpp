#include "const_scorer.h"

namespace yas {
ConstScorer::ConstScorer(float score) score_(score) {}
float ConstScorer::score() { return score_; }
}  // namespace yas