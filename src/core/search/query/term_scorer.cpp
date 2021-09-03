#include "term_scorer.h"

namespace yas {
TermScorer::TermScorer(
    std::shared_ptr<TermReader> reader, Relevance* rel, IndexStat index_stat,
    std::shared_ptr<FieldValueIndexReader> field_value_reader)
    : term_reader_(reader), field_value_reader_(field_value_reader) {
  auto weak_reader = term_reader_.lock();
  TermStat ts(Term(), reader->doc_freq(), 20);
  rel_scorer_ = rel->scorer(1.0, index_stat, ts);
}

TermScorer::~TermScorer() { delete rel_scorer_; }

float TermScorer::score() {
  uint64_t norm = 1;
  auto reader = term_reader_.lock();
  if (field_value_reader_) field_value_reader_->get(reader->docid(), norm);
  if (norm == -1) norm = 1;
  return rel_scorer_->score(reader->freq(), norm);
}
}  // namespace yas