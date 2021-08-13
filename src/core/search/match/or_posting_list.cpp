#include "or_posting_list.h"

#include <numeric>

namespace yas {
OrPostingList::OrPostingList(std::vector<PostingList*>& pls) {
  for (auto&& pl : pls) {
    pq_.push(pl);
  }
}

OrPostingList::~OrPostingList() {}

uint32_t OrPostingList::next() {
  if (pq_.size() == 0) return NDOCID;

  PostingList* top = pq_.top();
  uint32_t doc = top->docid();
  if (doc == NDOCID) return NDOCID;
  while (top->docid() == doc) {
    top->next();
    pq_.pop();
    pq_.push(top);
    top = pq_.top();
  }
  return pq_.top()->docid();
}

uint32_t OrPostingList::advance(uint32_t target) {
  if (pq_.size() == 0) return NDOCID;

  PostingList* top = pq_.top();
  while (top->docid() < target) {
    top->advance(target);
    pq_.pop();
    pq_.push(top);
    top = pq_.top();
  }
  return pq_.top()->docid();
}

uint32_t OrPostingList::docid() {
  if (pq_.size() == 0)
    return NDOCID;
  else
    return pq_.top()->docid();
}

long OrPostingList::cost() {
  return std::accumulate(
      pq_.begin(), pq_.end(), 0L,
      [](long a, const PostingList* b) { return a + b->cost(); });
}

std::string OrPostingList::name() { return "OrPostingList"; }

float OrPostingList::score() {
  if (pq_.size() == 0) return 0.0;
  float sum = 0.0;
  uint32_t doc = 0;

  std::vector<PostingList*> pls;
  PostingList* top = nullptr;
  do {
    top = pq_.top();
    sum += top->score();
    doc = top->docid();
    pq_.pop();
    pls.push_back(top);
  } while (top->docid() == doc);

  for (auto p : pls) {
    pq_.push(p);
  }
  return sum;
}
}  // namespace yas
