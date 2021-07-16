#include "none_match_query.h"
#include "const_scorer.h"
#include "none_posting_list.h"

namespace yas {
NoneMatchQuery::NoneMatchQuery(/* args */) {}

NoneMatchQuery::~NoneMatchQuery() {}

Query* NoneMatchQuery::rewrite() { return this; }

PostingList* NoneMatchQuery::posting_list(SubIndexReader* sub_reader) {
  return new NonePostingList();
}

Scorer* NoneMatchQuery::scorer() { return new ConstScorer(0.0); }
}  // namespace yas