#include "term_query.h"
#include "term_matcher.h"

namespace yas {
TermQuery::TermQuery(Term* term) :term_(term) {}
TermQuery::~TermQuery() {}
Query* TermQuery::rewrite(Query* query){ return this; }

std::unique_ptr<Matcher> TermQuery::matcher(SubIndexReader* sub_reader) {
  TermReader* term_reader = sub_reader->posting_list(term_);
  return std::unique_ptr<Matcher>(
      TermMatcher(term_reader, new BM25Relevance()));
}
}  // namespace yas