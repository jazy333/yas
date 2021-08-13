#include "sequence_posting_list.h"

#include <algorithm>
#include <iterator>

namespace yas {
SequencePostingList::SequencePostingList(std::vector<uint32_t>& docids)
    : docids_(docids), index_(0), docid_(0) {}

SequencePostingList::~SequencePostingList() {}

uint32_t SequencePostingList::next() {
  // for the first next
  if (docid_ == 0 && index_ < docids_.size()) {
    docid_++;
    return docids_[index_];
  }

  if (index_ + 1 < docids_.size()) {
    return docids_[++index_];
  } else {
    return NDOCID;
  }
}

uint32_t SequencePostingList::advance(uint32_t target) {
  auto iter = std::lower_bound(docids_.begin() + index_, docids_.end(), target);
  if (iter != docids_.end()) {
    index_ = std::distance(docids_.begin(), iter);
    return docids_[index_];
  } else
    return NDOCID;
}

uint32_t SequencePostingList::docid() {
  if (index_ < docids_.size())
    return docids_[index_];
  else
    return NDOCID;
}

long SequencePostingList::cost() { return docids_.size(); }

float SequencePostingList::score() { return 0.0; }

std::string SequencePostingList::name() { return "SequencePostingList"; }

}  // namespace yas