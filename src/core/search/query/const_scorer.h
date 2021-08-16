#pragma once
#include "scorer.h"

namespace yas {
class ConstScorer : public Scorer {
 public:
  ConstScorer(float score);
  virtual ~ConstScorer() = default;
  float score() override;

 private:
  float score_;
};
}  // namespace yas