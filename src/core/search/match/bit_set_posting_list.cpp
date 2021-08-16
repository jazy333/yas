#include "bit_set_posting_list.h"
#include <utility>

namespace yas {
BitSetPostingList::BitSetPostingList(std::unique_ptr<BitSet> bs)
    : bs_(std::move(bs)),docid_(0),cost_(bs->size()) {}

BitSetPostingList::~BitSetPostingList() {}

uint32_t BitSetPostingList::next() { return advance(docid_ + 1); }
uint32_t BitSetPostingList::advance(uint32_t target) {
  docid_ = bs_->next(target);
  return docid_;
}

uint32_t BitSetPostingList::docid() { return docid_; }

long BitSetPostingList::cost() { return cost_; }

std::string BitSetPostingList::name() { return "BitSetPostingList"; }

float BitSetPostingList::score() { return 0.0; }
}  // namespace yas