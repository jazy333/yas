#pragma once

#include "matcher.h"
#include "term_reader.h"
#include "relevance.h"
#include "term_scorer.h"

namespace yas {
class TermMatcher : public Matcher {
 public:
  TermMatcher(TermReader* term_reader, Relevance* rel,IndexStat* index_stat,
             TermStat* term_stat);
  virtual ~TermMatcher();
  PostingList* posting_list() override;
  Scorer* scorer() override;

 private:
  TermReader* term_reader_;
  Relevance* rel_;
  TermScorer* scorer_;
};
}  // namespace yas