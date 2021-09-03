#include "none_match_query.h"

#include "const_scorer.h"
#include "default_matcher.h"
#include "none_posting_list.h"

namespace yas {
NoneMatchQuery::NoneMatchQuery(/* args */) {}

NoneMatchQuery::~NoneMatchQuery() {}

std::shared_ptr<Query> NoneMatchQuery::rewrite() { return nullptr; }

std::shared_ptr<Matcher> NoneMatchQuery::matcher(SubIndexReader* sub_reader) {
  return std::shared_ptr<Matcher>(
      new DefaultMatcher(std::shared_ptr<PostingList>(new NonePostingList()),
                         std::shared_ptr<Scorer>(new ConstScorer(0.0))));
}

}  // namespace yas