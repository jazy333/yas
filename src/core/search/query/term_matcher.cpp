#include "term_matcher.h"

namespace yas {
TermMatcher::TermMatcher(
    TermReader* term_reader, Relevance* rel, IndexStat index_stat,
    std::shared_ptr<FieldValueIndexReader> field_value_reader)
    : term_reader_(term_reader), rel_(rel) {
  auto scorer =
      new TermScorer(term_reader_, rel_, index_stat, field_value_reader);
  term_reader_->set_scorer(scorer);
}

TermMatcher::~TermMatcher() { delete rel_; }

PostingList* TermMatcher::posting_list() { return term_reader_; }

Scorer* TermMatcher::scorer() { return term_reader_; }
}  // namespace yas