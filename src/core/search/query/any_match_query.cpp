#include "any_match_query.h"
#include "any_posting_list.h"
#include "const_scorer.h"
#include "default_matcher.h"

namespace yas {
Query* AnyMatchQuery::rewrite() {}

Matcher* AnyMatchQuery::matcher(SubIndexReader* sub_reader) {
  return new DefaultMatcher(new AnyPostingList(), new ConstScorer(0.0));
}
}  // namespace yas