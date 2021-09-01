#include "term_scorer.h"

namespace yas {
TermScorer::TermScorer(
    TermReader* reader, Relevance* rel, IndexStat index_stat,
    std::shared_ptr<FieldValueIndexReader> field_value_reader)
    : term_reader_(reader), field_value_reader_(field_value_reader) {
  TermStat ts(Term(), term_reader_->doc_freq(), 20);
  rel_scorer_ = rel->scorer(1.0, index_stat, ts);
}

TermScorer::~TermScorer() { delete rel_scorer_; }

float TermScorer::score() {
  uint64_t norm = 1;
  if (field_value_reader_)
    field_value_reader_->get(term_reader_->docid(), norm);
  if (norm == -1) norm = 1;
  return rel_scorer_->score(term_reader_->freq(), norm);
}
}  // namespace yas