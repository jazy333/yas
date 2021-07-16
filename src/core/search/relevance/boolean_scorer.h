#include "relevance_scorer.h"

namespace yas {
class BooleanScorer : public RelevanceScorer {
 public:
  BooleanScorer(float boost);
  ~BooleanScorer() = default;
  virtual float score(long freq, long norm) override;

 private:
  float boost_;
};
}  // namespace yas