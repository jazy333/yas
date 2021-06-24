#pragma once
#include "posting_list.h"
#include "match_set.h"

namespace yas {
class Matcher {
 public:
  Matcher(Query*);
  virtual int match(MatchSet& set, int min, int max) = 0;

 protected:
  Query* query_;
};
}  // namespace yas