#include "none_match_query.h"
#include "const_scorer.h"
#include "default_matcher.h"
#include "none_posting_list.h"

namespace yas {
NoneMatchQuery::NoneMatchQuery(/* args */) {}

NoneMatchQuery::~NoneMatchQuery() {}

Query* NoneMatchQuery::rewrite() { return this; }

Matcher* NoneMatchQuery::matcher(SubIndexReader* sub_reader) {
  return new DefaultMatcher(new NonePostingList(), new ConstScorer(0.0));
}

}  // namespace yas