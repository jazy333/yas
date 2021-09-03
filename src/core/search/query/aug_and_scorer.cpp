#include "aug_and_scorer.h"

namespace yas {
AugAndScorer::AugAndScorer(std::shared_ptr<PostingList> pl,
                           std::shared_ptr<PostingList> aux)
    : pl_(pl), aux_(aux) {}
float AugAndScorer::score() {
  float s = pl_->score();
  uint32_t docid = pl_->docid();
  uint32_t next = aux_->advance(docid);
  if (docid == next) {
    s += aux_->score();
  }
  return s;
}
}  // namespace yas
