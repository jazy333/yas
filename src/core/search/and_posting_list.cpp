#include "and_posting_list.h"

#include <limits>

namespace yas {
AndPostingList::AndPostingList(/* args */) {}

AndPostingList::~AndPostingList() {}

uint32_t AndPostingList::do_next(uint32_t next1) {
  while (true) {
    while (true) {
      uint32_t next2 = lead2->advance(docid);
      if (next1 != next2) {
        next1 = lead1->advance(next2);
        if (next1 == std::numeric_limits<uint32_t>::max) return next1;
        continue;
      }

      bool end = true;
      for (auto&& other : others) {
        if (other->docid() < next1) {
          uint32_t next = other->advance(next1);
          if (next > next1) {
            next1 = lead1->advance(next);
            if (next1 == std::numeric_limits<uint32_t>::max) return next1;
            end = false;
          }
        }
      }
      if (end) break;
    }
  }
  return next1;
}

uint32_t AndPostingList::do_next() { return do_next(lead1->next()); }

uint32_t AndPostingList::next() { return do_next(); }
uint32_t AndPostingList::advance(uint32_t target) {
  do_next(lead1->advance(target));
}
uint32_t AndPostingList::docid() { return lead1->docid(); }
virtual long AndPostingList::cost() { return lead1->cost(); }
};
}  // namespace yas