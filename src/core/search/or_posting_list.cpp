#include "or_posting_list.h"

#include <numeric>

namespace yas {
OrPostingList::OrPostingList(/* args */) {}

OrPostingList::~OrPostingList() {}

uint32_t OrPostingList::next() {
  do {
    PostingList* top = pq_->top();
    uint32_t doc = top->docid();
    top->next();
    pq_->pop();
    pq_->push(top);
  } while (top->docid() == doc);
  return pq_.top()->docid();
}

uint32_t OrPostingList::advance(uint32_t target) {
  do {
    PostingList* top = pq_->top();
    uint32_t next = top->advance(target);
    pq_->pop();
    pq_->push(top);
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
}  // namespace yas