#include "default_matcher.h"

namespace yas {
DefaultMatcher::DefaultMatcher(/* args */) {}
DefaultMatcher::~DefaultMatcher() {}

int DefaultMatcher::match(MatchSet& set, int min, int max) {
  DocIdList* list = query->docid_list();
  while (list->next()) {
    set.add(list->docid());
  }
}

}  // namespace yas