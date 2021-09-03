#pragma once
#include "and_posting_list.h"
#include "scorer.h"

namespace yas {
class AugAndScorer : public Scorer {
 public:
  AugAndScorer(std::shared_ptr<PostingList> pls,
               std::shared_ptr<PostingList> aux);
  virtual ~AugAndScorer() = default;
  float score() override;

 private:
  std::shared_ptr<PostingList> pl_;
  std::shared_ptr<PostingList> aux_;
};
}  // namespace yas