#pragma once
#include "and_posting_list.h"
#include "scorer.h"

namespace yas {
class AugAndScorer: public Scorer {
 public:
  AugAndScorer(PostingList* pls,PostingList* aux);
  virtual ~AugAndScorer() = default;
  float score() override;
  private:
  PostingList* pls_;
  PostingList* aux_;
};
}  // namespace yas