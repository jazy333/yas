#pragma once
#include "relevance.h"
#include "relevance_scorer.h"
#include "scorer.h"
#include "term_reader.h"

namespace yas {
class TermScorer : public Scorer {
 public:
  TermScorer(TermReader* reader, Relevance* rel, IndexStat index_stat);
  virtual ~TermScorer();
  float score() override;

 private:
  TermReader* term_reader_;
  RelevanceScorer* rel_scorer_;
};
}  // namespace yas