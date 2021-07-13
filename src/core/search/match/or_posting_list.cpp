#include "or_posting_list.h"

#include <numeric>

namespace yas {
OrPostingList::OrPostingList(std::vector<PostingList*> pls) {
  for (auto&& pl : pls) {
    pq_.push(pl);
  }
}

OrPostingList::~OrPostingList() {}

uint32_t OrPostingList::next() {
  do {
    PostingList* top = pq_->top();
    uint32_t doc = top->docid();
    top->next();
    pq_.pop();
    pq_.push(top);
  } while (top->docid() == doc);
  return pq_.top()->docid();
}

uint32_t OrPostingList::advance(uint32_t target) {
  do {
    PostingList* top = pq_->top();
    uint32_t next = top->advance(target);
    pq_.pop();
    pq_.push(top);
  } while (top->docid() < target);
  return pq_.top()->docid();
}

uint32_t OrPostingList::docid() { return pq_.top()->docid(); }

long OrPostingList::cost() {
  return std::accumulate(
      pq_.begin(), pq_.end(), 0L,
      [](long a, const PostingList* b) { return a + b->cost(); });
}

std::string OrPostingList::name() { return "OrPostingList"; }

float OrPostingList::score() {
  float sum = 0.0;
  uint32_t doc = 0;
  std::vector<PostingList*> pls;
  do {
    PostingList* top = pq_->top();
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