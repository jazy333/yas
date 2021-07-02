#include "default_matcher.h"

namespace yas {
DefaultMatcher::DefaultMatcher(/* args */) {}
DefaultMatcher::~DefaultMatcher() {}

int DefaultMatcher::match(MatchSet& set, int min, int max) {
  PostingList* list = query->docid_list();
  //list->advance(min);
  while (list->next() != std::numeric_limits<uint32_t>::max) {
    set.add(list->docid());
  }
}

}  // namespace yas