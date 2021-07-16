#include "term_scorer.h"

namespace yas {
TermScorer::TermScorer(TermReader* reader, Relevance* rel,
                       IndexStat* index_stat, TermStat* term_stat)
    : reader_(reader) {
  rel_scorer_ = rel->scorer(index_stat, term_stat);
}

TermScorer::~TermScorer() {}

float TermScorer::score() {
  return rel_scorer_->score(reader_->freq(), reader_->docid());
}
}  // namespace yas