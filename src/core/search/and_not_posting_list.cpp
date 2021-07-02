#include "and_not_posting_list.h"

namespace yas {
AndNotPostingList::AndNotPostingList(PostingList* pl, PostingList* excl)
    : pl_(pl), excl_(excl) {}

AndNotPostingList::~AndNotPostingList() {}

uint32_t AndNotPostingList::do_next(uint32_t next) {
  while (next >= excl_->docid()) {
    if (next == std::numeric_limits<uint32_t>::max ||
        excl_->docid() == std::numeric_limits<uint32_t>::max) {
      return next;
    }
    if (next == excl_->docid())
      next = pl_->next();
    else
      excl_->advance(next);
  }
  return next;
}

uint32_t AndNotPostingList::next() {
  uint32_t next = pl_->next();
  return do_next(next);
}

uint32_t AndNotPostingList::advance(uint32_t target) {
  uint32_t next = pl_->advance(target);
  return do_next(next);
}

uint32_t AndNotPostingList::docid() {
  do_next(pl_->docid());
  return pl_->docid();
}

long AndNotPostingList::cost() { return pl_->cost(); }

std::string AndNotPostingList::name() { return "AndNotPostingList"; }

float AndNotPostingList::score() { return pl_->score(); }
}  // namespace yas