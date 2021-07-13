#pragma once
#include "matcher.h"

namespace yas {
class DefaultMatcher : public Matcher {

 public:
  DefaultMatcher(PostingList*,Scorer*);
  ~DefaultMatcher();
  PostingList* posting_list() override;
  Scorer* scorer() override;
  private:
  PostingList* pl_;
  Scorer scorer_;
};
}  // namespace yas