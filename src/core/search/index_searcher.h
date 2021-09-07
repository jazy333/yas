#pragma once

#include <memory>

#include "core/index/index_reader.h"
#include "core/search/match/match_set.h"
#include "core/search/query/query.h"
#include "core/search/relevance/relevance.h"

namespace yas {
class IndexSearcher {
 public:
  IndexSearcher(IndexReader* reader);
  virtual ~IndexSearcher();
  void search(Query* q, MatchSet& set);
  std::shared_ptr<Query> rewrite(std::shared_ptr<Query> query);

 private:
  IndexReader* reader_;
};

}  // namespace yas