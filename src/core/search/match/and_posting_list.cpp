#include "and_posting_list.h"

namespace yas {
AndPostingList::AndPostingList(std::vector<PostingList*>& pls)
    : lead1_(nullptr), lead2_(nullptr) {
  if (pls.size() == 0) {
    // do nothing
  } else if (pls.size() == 1) {
    lead1_ = pls[0];

  } else {
    lead1_ = pls[0];
    lead2_ = pls[1];

    if (lead1_->cost() > lead2_->cost()) {
      std::swap(lead1_, lead2_);
    }

    long cost1 = lead1_->cost();
    long cost2 = lead2_->cost();

    for (int i = 2; i < pls.size(); ++i) {
      long cur_cost = pls[i]->cost();
      if (cost1 > cur_cost) {
        others_.push_back(lead1_);
        lead1_ = pls[i];
        cost1 = cur_cost;
      } else {
        if (cost2 > cur_cost) {
          others_.push_back(lead2_);
          lead2_ = pls[i];
          cost2 = cur_cost;
        } else
          others_.push_back(pls[i]);
      }
    }
  }
}

AndPostingList::~AndPostingList() {}

uint32_t AndPostingList::do_next(uint32_t next1) {
  while (true) {
    // find the less one between lead1 and lead2
    // TODO:SIMD OPT
    uint32_t next2 = lead2_->advance(next1);
    if (next1 != next2) {
      next1 = lead1_->advance(next2);
      if (next1 == NDOCID) return next1;
      continue;
    }

    bool found = true;
    for (auto&& other : others_) {
      if (other->docid() < next1) {
        // do not return when next==NDOCID,use it to advance lead1
        uint32_t next = other->advance(next1);
        if (next > next1) {
          next1 = lead1_->advance(next);
          if (next1 == NDOCID) return next1;
          found = false;
          break;
        }
      }
    }
    if (found) break;
  }

  return next1;
}

uint32_t AndPostingList::do_next() {
  return lead1_ ? (lead2_ ? do_next(lead1_->next()) : lead1_->next()) : NDOCID;
}

uint32_t AndPostingList::next() { return do_next(); }

uint32_t AndPostingList::advance(uint32_t target) {
  return lead1_ ? (lead2_ ? do_next(target) : lead1_->advance(target)) : NDOCID;
}

uint32_t AndPostingList::docid() { return lead1_ ? lead1_->docid() : NDOCID; }

long AndPostingList::cost() { return lead1_ ? lead1_->cost() : 0; }

std::string AndPostingList::name() { return "AndPostingList"; }

float AndPostingList::score() {
  float sum = 0.0;
  if (lead1_ && lead2_)
    sum += lead1_->score() + lead2_->score();
  else if (lead1_)
    sum += lead1_->score();

  for (auto p : others_) {
    sum += p->score();
  }
  return sum;
}
}  // namespace yas