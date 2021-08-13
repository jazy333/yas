#pragma once

#include <memory>

#include "index_reader.h"
#include "match_set.h"
#include "query.h"
#include "relevance.h"

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