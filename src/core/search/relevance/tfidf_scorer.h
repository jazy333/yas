#pragma once

#include "relevance_scorer.h"

namespace yas {
class TFIDFScorer : public RelevanceScorer {
 public:
  TFIDFScorer(float boost, float idf);
  virtual float score(long freq, long norm) override;

 private:
  float boost_;
  float idf_;
};
}  // namespace yas