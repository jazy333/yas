#include "or_posting_list.h"

#include <numeric>

namespace yas {
OrPostingList::OrPostingList(std::vector<std::shared_ptr<PostingList>>& pls)
    : cost_(0), docid_(0) {
  for (auto&& pl : pls) {
    pq_.push(pl);
    cost_ += pl->cost();
  }

  if (pls.size() == 0) {
    docid_ = NDOCID;
  }
}

OrPostingList::~OrPostingList() {}

uint32_t OrPostingList::next() {
  if (pq_.size() == 0 || docid_ == NDOCID) return NDOCID;

  auto top = pq_.top();
  while (top->docid() == docid_) {
    top->next();
    pq_.pop();
    pq_.push(top);
    top = pq_.top();
  }
  docid_ = pq_.top()->docid();
  return docid_;
}

uint32_t OrPostingList::advance(uint32_t target) {
  if (pq_.size() == 0 || docid_ == NDOCID) return NDOCID;

  auto top = pq_.top();
  while (top->docid() < target) {
    top->advance(target);
    pq_.pop();
    pq_.push(top);
    top = pq_.top();
  }
  docid_ = pq_.top()->docid();
  return docid_;
}

uint32_t OrPostingList::docid() {
  if (pq_.size() == 0)
    return NDOCID;
  else
    return pq_.top()->docid();
}

long OrPostingList::cost() { return cost_; }

std::string OrPostingList::name() { return "OrPostingList"; }

float OrPostingList::score() {
  if (pq_.size() == 0) return 0.0;
  float sum = 0.0;
  uint32_t doc = 0;

  std::vector<std::shared_ptr<PostingList>> pls;
  std::shared_ptr<PostingList> top = nullptr;
  do {
    top = pq_.top();
    doc = top->docid();
    if (doc != docid_) break;
    sum += top->score();
    pq_.pop();
    pls.push_back(top);
  } while (pq_.size() > 0);

  for (auto p : pls) {
    pq_.push(p);
  }
  return sum;
}

std::unordered_map<std::string, int> OrPostingList::hit() {
  if (pq_.size() == 0) return std::unordered_map<std::string, int>();
  std::unordered_map<std::string, int> counts;
  std::vector<std::shared_ptr<PostingList>> pls;
  std::shared_ptr<PostingList> top = nullptr;
  uint32_t doc = 0;
  do {
    top = pq_.top();
    doc = top->docid();
    if (doc != docid_) break;
    auto hits = top->hit();
    for (auto kv : hits) {
      if (counts.count(kv.first) == 1) {
        counts[kv.first] += kv.second;
      } else {
        counts[kv.first] = kv.second;
      }
    }
    pq_.pop();
    pls.push_back(top);
  } while (pq_.size() > 0);

  for (auto p : pls) {
    pq_.push(p);
  }
  return counts;
}
}  // namespace yas
