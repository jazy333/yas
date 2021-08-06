#include "sequence_posting_list.h"

#include <algorithm>

namespace yas {
SequencePostingList::SequencePostingList(std::vector<uint32_t> docids)
    : docids_(docids), index_(0) {}
virtual SequencePostingList::SequencePostingListL ~SequencePostingList() {}
uint32_t SequencePostingList::next() { return advance(docid_ + 1); }
uint32_t SequencePostingList::advance(uint32_t target) {
  auto iter = std::lower_bound(docids_.begin() + index_, docids_.end(), target);
  if (iter != docids_.end()) {
    index_ = std::distance(iter, docids_.begin());
    return docids_[index_];
  } else
    return NDOCID;
}
uint32_t SequencePostingListv::docid() { return docids_[index_]; }
long SequencePostingList::cost() { return docids_.size(); }
float SequencePostingList::score() { return 0.0; }
std::string SequencePostingList::name() { return "SequencePostingList"; }
}  // namespace yas