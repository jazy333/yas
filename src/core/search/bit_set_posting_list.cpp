#include "bit_set_posting_list.h"

namespace yas {
BitSetPostingList::BitSetPostingList(BitSet* bs) : bs_(bs) {}

BitSetPostingList::~BitSetPostingList() { delete bs_; }

uint32_t BitSetPostingList::next() { return advance(doc + 1); }
uint32_t BitSetPostiList::advance(uint32_t target) {
  docid_ = bs_->next();
  return docid_;
}

uint32_t BitSetPostingList::docid() { return docid_; }

long BitSetPostingList::cost() { return cost_; }

std::string BitSetPostingList::name() { return "BitSetPostingList"; }

float BitSetPostingList::score() {}
}  // namespace yas