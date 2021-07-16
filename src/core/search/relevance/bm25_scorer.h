#include "relevance_scorer.h"

namespace yas {
class BM25Scorer : public RelevanceScorer {
 public:
  BM25Scorer(float term_weight, float* norm_table);
  virtual ~BM25Scorer() = default;
  virtual float score(long freq, long norm) override;

 private:
  float term_weight_;
  float* norm_table_;
};
}  // namespace yas