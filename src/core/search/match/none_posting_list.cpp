#include "none_posting_list.h"

namespace yas {

NonePostingList::NonePostingList(/* args */) {}

NonePostingList::~NonePostingList() {}
uint32_t NonePostingList::next() { return NDOCID; }
uint32_t NonePostingList::advance(uint32_t target) { return NDOCID; }
uint32_t NonePostingList::docid() { return NDOCID; }
long NonePostingList::cost() { return 0; }
std::string NonePostingList::name() { return "NonePostingList"; }

}  // namespace yas