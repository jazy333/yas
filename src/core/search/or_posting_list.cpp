#include "or_posting_list.h"

#include <numeric>

namespace yas {
OrPostingList::OrPostingList(/* args */) {}

OrPostingList::~OrPostingList() {}

bool OrPostingList::posting_list_compare(PostingList* l, PostingList* r) {}
uint32_t OrPostingList::next() {
  std::priority_queue<PostingList*> pq;
  for (PostingList* pl : pq_) {
    pl->next();
    pq.push(pl);
  }
  pq_ = pq;
  return pq_.top()->docid();
}
uint32_t OrPostingList::advance(uint32_t target) {}
uint32_t OrPostingList::docid() {
  std::priority_queue<PostingList*> pq;
  for (PostingList* pl : pq_) {
    pl->advance(target);
    pq.push(pl);
  }
  pq_ = pq;
  return pq_.top->docid();
}
virtual long OrPostingList::cost() {
  return std::accumulate(
      pq_.begin(), pq_.end(), 0L,
      [](long a, const PostingList* b) { return a + b->cost(); });
}
}  // namespace yas