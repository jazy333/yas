#include "aug_and_posting_list.h"

namespace yas {
AugAndScorer::AugAndScorer(PostingList* pl,PostingList* aux):pl_(pl),aux_(aux) {}
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
