#include "any_match_query.h"

#include "any_posting_list.h"
#include "const_scorer.h"
#include "default_matcher.h"

namespace yas {
Query* AnyMatchQuery::rewrite() { return this; }

std::unique_ptr<Matcher> AnyMatchQuery::matcher(SubIndexReader* sub_reader) {
  return std::unique_ptr<Matcher>(
      new DefaultMatcher(new AnyPostingList(NDOCID - 1), new ConstScorer(0.0)));
}
}  // namespace yas