#pragma once

#include "index_reader.h"
#include "match_set.h"
#include "query.h"
#include "relevance.h"

#include <memory>

namespace yas {
class IndexSearcher {
 private:
  IndexReader* reader_;
 
 public:
  IndexSearcher(IndexReader* reader);
  virtual ~IndexSearcher();
  void search(Query* q, MatchSet& set);
  std::shared_ptr<Query> rewrite(std::shared_ptr<Query> query);
};

}  // namespace yas