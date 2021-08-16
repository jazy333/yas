#include "bit_set_posting_list.h"

namespace yas {
BitSetPostingList::BitSetPostingList(BitSet* bs) : bs_(bs) {}

BitSetPostingList::~BitSetPostingList() { delete bs_; }

uint32_t BitSetPostingList::next() { return advance(docid_ + 1); }
uint32_t BitSetPostingList::advance(uint32_t target) {
  docid_ = bs_->next(target);
  return docid_;
}

uint32_t BitSetPostingList::docid() { return docid_; }

long BitSetPostingList::cost() { return bs_->size(); }

std::string BitSetPostingList::name() { return "BitSetPostingList"; }

float BitSetPostingList::score() { return 0.0; }
}  // namespace yas