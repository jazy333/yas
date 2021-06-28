#include "mm_posting_list.h"

#include <functional>

namespace yas {
MMPostingList::MMPostingList(std::vector<PostingList*>& pl, int minimum_match)
    : minimum_match_(minimum_match), docid_(0) {
  for (auto p : pl) {
    add_lead(*p);
  }

  std::priority_queue<PostingList*, posting_list_compare_cost> tail;
  for (auto p : pl) {
    if (tail.size() < pl.size() - minimum_match_ + 1) {
      tail.push(*p);
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
MMPostingList::~MMPostingList() {}

void MMPostingList::add_lead(PostingList* pl) {
  lead_.push_front(pl);
  matched_++;
}

void MMPostingList::update_lead() {
  lead_.clear();
  matched_ = 1;
  PostingList* top = head_.top();
  lead_.push_back(top);
  head_.pop();
  docid_ = top->docid();
  while (head_.size() && head_.top()->docid() == docid_) {
    lead_.push_back(head_.top());
    head_.pop();
    matched_++;
  }
}

uint32_t MMPostingList::do_next() {
  while (matched_ < minimum_match_) {
    if (matched_ + tail_.size() >= minimum_match_) {
      PostingList* top = tail_.top();
      tail_.pop();
      top->advance(docid_);
      if (top->docid() == docid_)
        add_lead(top);
      else
        head_.push(top);
    } else {
      for (auto p : lead_) {
        tail_.push(p);
      }
      lead_.clear();
      update_lead();
    }
  }
  return docid_;
}

PostingList* MMPostingList::add_tail(PostingList* p) {
  if (tail_.size() < minimum_match_ - 1) {
    tail_.push(*p);
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

uint32_t MMPostingList::next() {
  for (auto p : lead_) {
    PostingList* cur = add_tail(p);
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

uint32_t MMPostingList::advance(uint32_t target) {
  for (auto p : lead_) {
    PostingList* cur = add_tail(p);
    if (cur) {
      cur->advance(target);
      head_.push(cur);
    }
  }

  while (head_.top()->docid() < target) {
    auto top = head_.top();
    head_.pop();
    top->advance(target);
    auto cur = add_tail(top);
    cur->advance(target);
    head_.push(cur);
  }

  update_lead();
  while (matched_ + tail_.size() < minimum_match_) {
    // no match on doc is possible, move to the next potential match
    for (auto p : lead_) {
      tail_.push(p);
    }
    lead_.clear();
    update_lead();
  }
  return docid_;
}

uint32_t MMPostingList::docid() { return docid_; }
long MMPostingList::cost() { return cost_; }
std::string MMPostingList::name() { return "MMPostingList"; }
}  // namespace yas