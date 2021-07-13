#include "default_matcher.h"

namespace yas {
DefaultMatcher::DefaultMatcher(PostingList* pl, Scorer* scorer)
    : pl_(pl), scorer_(scorer) {}
DefaultMatcher::~DefaultMatcher() {}

PostingList* DefaultMatcher::posting_list() { return pl_; }
Scorer* DefaultMatcher::scorer() { return scorer_; }
}

}  // namespace yas