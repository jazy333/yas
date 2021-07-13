#pragma once
#include "relevance.h"
#include "scorer.h"
#include "term_reader.h"

namespace yas {
class TermScorer : public Scorer {
 public:
  TermScorer(TermReader*, Relevance*);
  ~TermScorer();
  float score() override;

 private:
  TermReader* reader_;
  Relevance* rel_;
};
}  // namespace yas