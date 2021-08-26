#include "term_matcher.h"

namespace yas {
TermMatcher::TermMatcher(TermReader* term_reader, Relevance* rel,
                         IndexStat index_stat)
    : term_reader_(term_reader), rel_(rel) {
  scorer_ = new TermScorer(term_reader_, rel_, index_stat);
}

TermMatcher::~TermMatcher() {
  delete rel_;
  delete scorer_;
}

PostingList* TermMatcher::posting_list() { return term_reader_; }
Scorer* TermMatcher::scorer() { return scorer_; }
}  // namespace yas