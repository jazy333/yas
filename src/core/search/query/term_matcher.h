#pragma once

#include "matcher.h"

namespace yas {
class TermMatcher : public Matcher {
 public:
  TermMatcher(TermReader* term_reader, Relevance* rel);
  virtual ~TermMatcher();
  PostingList* posting_list() override;
  Scorer* scorer() override;

 private:
  TermReader* term_reader;
  Relevance* rel_;
  TermScorer* scorer_;
};
}  // namespace yas