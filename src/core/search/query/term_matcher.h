#pragma once

#include "matcher.h"

namespace yas {
class TermMatcher : public Matcher {
 public:
  TermMatcher(TermReader*, Relevance*);
  ~TermMatcher();
  PostingList* posting_list() override;
  Scorer* scorer() override;

 private:
  TermReader* term_reader;
  Relevance* rel_;
};
}  // namespace yas