#include "term_scorer.h"

namespace yas {
TermScorer::TermScorer(TermReader* reader, Relevance* rel,
                       IndexStat index_stat)
    : term_reader_(reader) {
  TermStat ts(Term(), term_reader_->doc_freq(), 1);
  rel_scorer_ = rel->scorer(1.0, index_stat, ts);
}

TermScorer::~TermScorer() {}

float TermScorer::score() {
  return rel_scorer_->score(term_reader_->freq(), term_reader_->docid());
}
}  // namespace yas