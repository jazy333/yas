#pragma once
#include "relevance.h"
#include "relevance_scorer.h"
#include "scorer.h"
#include "term_reader.h"
#include "field_index_reader.h"

namespace yas {
class TermScorer : public Scorer {
 public:
  TermScorer(TermReader* reader, Relevance* rel, IndexStat index_stat,
             std::shared_ptr<FieldValueIndexReader> field_value_reader);
  virtual ~TermScorer();
  float score() override;

 private:
  TermReader* term_reader_;
  RelevanceScorer* rel_scorer_;
  std::shared_ptr<FieldValueIndexReader> field_value_reader_;
};
}  // namespace yas