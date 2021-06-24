#pragma once
#include "match_set.h"
#include "query.h"

namespace yas {
class IndexSearcher {
 private:
  /* data */
 public:
  IndexSearcher(/* args */);
  virtual ~IndexSearcher();
  void search(Query* q, MatchSet& set);
};

}  // namespace yas