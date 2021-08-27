#include "term_scorer.h"
#include <iostream>

namespace yas {
TermScorer::TermScorer(TermReader* reader, Relevance* rel,
                       IndexStat index_stat)
    : term_reader_(reader) {
  TermStat ts(Term(), term_reader_->doc_freq(), 20);
  rel_scorer_ = rel->scorer(1.0, index_stat, ts);
}

TermScorer::~TermScorer() {
  delete rel_scorer_;
}

float TermScorer::score() {
  return rel_scorer_->score(term_reader_->freq(), 3);
}
}  // namespace yas