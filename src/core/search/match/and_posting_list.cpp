#include "and_posting_list.h"

#include <limits>

namespace yas {
AndPostingList::AndPostingList(std::vector<PostingList*>& pls) {
  lead1_ = pls[0];
  lead2_ = pls[1];
  PostingList* min_posting_list = lead1_;
  long min_cost = lead1_->cost();
  if (lead1_->cost() > lead2_->cost()) {
    min_cost = lead2_->cost();
    min_posting_list = lead2_;
  }
  for (int i = 2; i < pls.size(); ++i) {
    if (min_cost > pls[i]->cost()) {
      others_.push_back(min_posting_list);
      min_posting_list = pls[i];
    } else {
      others_.push_back(pls[i]);
    }
  }
}

AndPostingList::~AndPostingList() {}

uint32_t AndPostingList::do_next(uint32_t next1) {
  while (true) {
    while (true) {
      uint32_t next2 = lead2_->advance(next1);
      if (next1 != next2) {
        next1 = lead1_->advance(next2);
        if (next1 == NDOCID) return next1;
        continue;
      }

      bool end = true;
      for (auto&& other : others_) {
        if (other->docid() < next1) {
          uint32_t next = other->advance(next1);
          if (next > next1) {
            next1 = lead1_->advance(next);
            if (next1 == NDOCID) return next1;
            end = false;
          }
        }
      }
      if (end) break;
    }
  }
  return next1;
}

uint32_t AndPostingList::do_next() { return do_next(lead1_->next()); }

uint32_t AndPostingList::next() { return do_next(); }

uint32_t AndPostingList::advance(uint32_t target) {
  do_next(lead1_->advance(target));
}

uint32_t AndPostingList::docid() { return lead1_->docid(); }

long AndPostingList::cost() { return lead1_->cost(); }

std::string AndPostingList::name() { return "AndPostingList"; }

float AndPostingList::score() {
  float sum = 0.0;
  sum += lead1_->score() + lead2_->score();
  for (auto p : others_) {
    sum += p->score();
  }
  return sum;
}
}  // namespace yas