#include "weak_and_posting_list.h"

#include <functional>
#include <vector>

namespace yas {
WeakAndPostingList::WeakAndPostingList(std::vector<std::shared_ptr<PostingList>>& pl,
                                       int minimum_match)
    : minimum_match_(minimum_match), docid_(0) {
  for (auto p : pl) {
    add_lead(p);
  }

  // compute cost
  std::priority_queue<std::shared_ptr<PostingList>, std::vector<std::shared_ptr<PostingList>>,
                      posting_list_compare_with_cost>
      tail;
  for (auto p : pl) {
    if (tail.size() < pl.size() - minimum_match_ + 1) {
      tail.push(p);
    } else {
      auto cur = tail.top();
      if (cur->cost() < p->cost()) {
        tail.pop();
        tail.push(p);
      }
    }
  }

  cost_ = 0;
  while (!tail.empty()) {
    auto p = tail.top();
    tail.pop();
    cost_ += p->cost();
  }
}

WeakAndPostingList::~WeakAndPostingList() {}

void WeakAndPostingList::add_lead(std::shared_ptr<PostingList> pl) {
  lead_.push_front(pl);
  matched_++;
}

void WeakAndPostingList::update_lead() {
  lead_.clear();
  matched_ = 1;
  // first matched doc
  auto top = head_.top();
  lead_.push_back(top);
  head_.pop();
  docid_ = top->docid();
  // find all matched doc in head_
  while (head_.size() && head_.top()->docid() == docid_) {
    lead_.push_back(head_.top());
    head_.pop();
    matched_++;
  }
}

uint32_t WeakAndPostingList::do_next() {
  // not get all docs
  while (matched_ < minimum_match_) {
    // still can get docs from tail
    if (matched_ + tail_.size() >= minimum_match_) {
      auto top = tail_.top();
      tail_.pop();
      top->advance(docid_);
      if (top->docid() == docid_)
        add_lead(top);
      else
        head_.push(top);  // the pre if guarentee the head_ size <=n-mm+1
    } else {              // no enough docs in tail,next doc

      // reback all nodes to tail_
      for (auto p : lead_) {
        tail_.push(p);
      }
      // recompute matched doc
      update_lead();
    }
  }
  return docid_;
}

std::shared_ptr<PostingList> WeakAndPostingList::add_tail(std::shared_ptr<PostingList> p) {
  if (tail_.size() < minimum_match_ - 1) {
    tail_.push(p);
    return nullptr;
  } else {
    auto cur = tail_.top();
    if (cur->cost() < p->cost()) {
      tail_.pop();
      tail_.push(p);
    } else
      cur = p;
    return cur;
  }
}

uint32_t WeakAndPostingList::next() {
  for (auto p : lead_) {
    auto cur = add_tail(p);
    if (cur) {
      if (cur->docid() == docid_) {
        cur->next();
      } else {
        cur->advance(docid_ + 1);
      }
      head_.push(cur);
    }
  }

  update_lead();
  return do_next();
}

uint32_t WeakAndPostingList::advance(uint32_t target) {
  for (auto p : lead_) {
    auto cur = add_tail(p);
    if (cur) {
      cur->advance(target);
      head_.push(cur);
    }
  }

  // make sure other pl's docid in head_ >= target
  while (head_.top()->docid() < target) {
    auto top = head_.top();
    head_.pop();
    top->advance(target);
    auto cur = add_tail(top);
    cur->advance(target);
    head_.push(cur);
  }

  update_lead();
  return do_next();
}

uint32_t WeakAndPostingList::docid() {
  return docid_;
}

long WeakAndPostingList::cost() { return cost_; }

std::string WeakAndPostingList::name() { return "WeakAndPostingList"; }

float WeakAndPostingList::score() {
  float sum = 0.0;
  for (auto f : lead_) {
    sum += f->score();
  }
  return sum;
}
}  // namespace yas