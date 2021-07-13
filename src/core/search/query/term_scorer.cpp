#include "term_scorer.h"

namespace yas {
TermScorer::TermScorer(TermReader* reader, Relevance* rel)
    : reader_(reader), rel_(rel) {}
TermScorer::~TermScorer() {}
float TermScorer::score() {
  return rel_->score(reader_->docid(), reader_->freq());
}
}  // namespace yas