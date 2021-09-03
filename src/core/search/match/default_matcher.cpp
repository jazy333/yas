#include "default_matcher.h"

namespace yas {
DefaultMatcher::DefaultMatcher(std::shared_ptr<PostingList> pl,
                               std::shared_ptr<Scorer> scorer)
    : pl_(pl), scorer_(scorer) {}

DefaultMatcher::~DefaultMatcher() {}

std::shared_ptr<PostingList> DefaultMatcher::posting_list() { return pl_; }

std::shared_ptr<Scorer> DefaultMatcher::scorer() { return scorer_; }
}  // namespace yas