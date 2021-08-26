#include "term_query.h"

#include "bm25_relevance.h"
#include "term_matcher.h"

namespace yas {
TermQuery::TermQuery(Term* term, IndexStat index_stat)
    : term_(term), index_stat_(index_stat){}

TermQuery::~TermQuery() {}

Query* TermQuery::rewrite() { return this; }

std::unique_ptr<Matcher> TermQuery::matcher(SubIndexReader* sub_reader) {
  auto invert_reader = sub_reader->invert_index_reader();
  TermReader* term_reader = invert_reader->get_reader(term_);
  return std::unique_ptr<Matcher>(
      new TermMatcher(term_reader, new BM25Relevance(), index_stat_));
}
}  // namespace yas