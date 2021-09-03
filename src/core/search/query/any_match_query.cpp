#include "any_match_query.h"

#include "any_posting_list.h"
#include "const_scorer.h"
#include "default_matcher.h"

namespace yas {
std::shared_ptr<Query> AnyMatchQuery::rewrite() { return nullptr; }

std::shared_ptr<Matcher> AnyMatchQuery::matcher(SubIndexReader* sub_reader) {
  return std::unique_ptr<Matcher>(new DefaultMatcher(
      std::shared_ptr<PostingList>(new AnyPostingList(NDOCID - 1)),
      std::shared_ptr<Scorer>(new ConstScorer(0.0))));
}
}  // namespace yas