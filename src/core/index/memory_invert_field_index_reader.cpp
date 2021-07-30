#include "memory_invert_field_index_reader.h"

#include <algorithm>

namespace yas {
MemoryInvertFieldIndexReader::MemoryInvertFieldIndexReader(
    std::vector<uint32_t>* posting_lists,
    std::vector<std::vector<int>>* postion_lists)
    : posting_lists_(posting_lists),
      position_lists_(position_lists),
      index_(-1),
      position_index_(0) {}

uint32_t MemoryInvertFieldIndexReader::next() {
  if (index_ >= posting_lists_.size()) return NDOCID;
  ++index_;
  position_index_ = 0;
  return posting_lists[index_];
}

uint32_t MemoryInvertFieldIndexReader::advance(uint32_t target) {
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

uint32_t MemoryInvertFieldIndexReader::docid() {
  if (index_ >= posting_lists_.size())
    return NDOCID;
  else
    return posting_lists_[index_];
}

long MemoryInvertFieldIndexReader::cost() { return posting_lists_.size(); }
std::string MemoryInvertFieldIndexReader::name() {
  return "MemoryInvertFieldIndexReader";
}

float MemoryInvertFieldIndexReader::score() { return 0.0f; }

Scorer* MemoryInvertFieldIndexReader::scorer() {}

int MemoryInvertFieldIndexReader::freq() {
  return position_lists_[index_].size();
}

int MemoryInvertFieldIndexReader::next_postion() {
  return position_lists[index_][position_index_++];
}

}  // namespace yas