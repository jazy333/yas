#include "any_posting_list.h"

namespace yas {
AnyPostingList::AnyPostingList(uint32_t max_docid) docid_(0),
    max_docid_(max_docid) {}

AnyPostingList::~AnyPostingList() {}

uint32_t AnyPostingList::next() { return advance(docid_ + 1); }

uint32_t AnyPostingList::advance(uint32_t target) {
  if (target > max_docid_)
    docid_ = NDOCID;
  else
    docid_ = target;
  return docid_;
}

uint32_t AnyPostingList::docid() { return docid_ }

long AnyPostingList::cost() { return max_docid_; }

std::string AnyPostingList::name() { return "AnyPostingList"; }

}  // namespace yas