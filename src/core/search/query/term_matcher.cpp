#include "term_matcher.h"

namespace yas {
TermMatcher::TermMatcher(TermReader* term_reader, Relevance* rel)
    : term_reader_(term_reader), rel_(rel) {
  score_ = new TermScore(term_reader_, rel_);
}

TermMatcher::~TermMatcher() {
  delete rel_;
  delete score_;
}

PostingList* TermMatcher::posting_list(SubIndexReader* sub_reader) {
  return term_reader_;
}
Scorer* TermMatcher::scorer() { return score_; }
}  // namespace yas