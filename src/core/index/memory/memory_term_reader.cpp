#include "memory_term_reader.h"

#include <algorithm>

namespace yas {
MemoryTermReader::MemoryTermReader(
    std::vector<uint32_t>& posting_lists,
    std::vector<std::vector<uint32_t>>& position_lists)
    : posting_lists_(posting_lists),
      position_lists_(position_lists),
      index_(-1),
      position_index_(0) {}

uint32_t MemoryTermReader::next() {
  if (index_ >= posting_lists_.size()) return NDOCID;
  ++index_;
  position_index_ = 0;
  return posting_lists_[index_];
}

uint32_t MemoryTermReader::advance(uint32_t target) {
  position_index_ = 0;
  auto iter =
      std::lower_bound(posting_lists_.begin(), posting_lists_.end(), target);
  if (iter != posting_lists_.end()) {
    index_ = std::distance(iter, posting_lists_.begin());
    return *iter;
  } else {
    return NDOCID;
  }
}

uint32_t MemoryTermReader::docid() {
  if (index_ >= posting_lists_.size())
    return NDOCID;
  else
    return posting_lists_[index_];
}

long MemoryTermReader::cost() { return posting_lists_.size(); }

std::string MemoryTermReader::name() { return "MemoryTermReader"; }

float MemoryTermReader::score() { return 0.0f; }

int MemoryTermReader::freq() { return position_lists_[index_].size(); }

int MemoryTermReader::next_postion() {
  return position_lists_[index_][position_index_++];
}

}  // namespace yass