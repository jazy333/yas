#include "not_posting_list.h"

namespace yas {
NotPostingList::NotPostingList(PostingList* pl, PostingList* excl)
    : pl_(pl), excl_(excl) {}

NotPostingList::~NotPostingList() {}

uint32_t NotPostingList::do_next(uint32_t next) {
  while (next >= excl_->docid()) {
    if (next == NDOCID || excl_->docid() == NDOCID) {
      return next;
    }
    if (next == excl_->docid())
      next = pl_->next();
    else
      excl_->advance(next);
  }
  return next;
}

uint32_t NotPostingList::next() {
  uint32_t next = pl_->next();
  return do_next(next);
}

uint32_t NotPostingList::advance(uint32_t target) {
  uint32_t next = pl_->advance(target);
  return do_next(next);
}

uint32_t NotPostingList::docid() {
  do_next(pl_->docid());
  return pl_->docid();
}

long NotPostingList::cost() { return pl_->cost(); }

std::string NotPostingList::name() { return "NotPostingList"; }

float NotPostingList::score() { return pl_->score(); }
}  // namespace yas