#include "term_posting_list.h"

namespace yas {
TermPostingList::TermPostingList(/* args */) {}

TermPostingList::~TermPostingList() {}

uint32_t TermPostingList::next() {}
uint32_t TermPostingList::advance(uint32_t target) {}
uint32_t TermPostingList::docid() {}
long TermPostingList::cost() {}
std::string TermPostingList::name() {
    return "TermPostingList";
}
float TermPostingList::score() {}

}  // namespace yas