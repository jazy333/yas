#pragma once

#include <memory>

#include "core/index/index_reader.h"
#include "core/search/match/match_set.h"
#include "core/search/query/query.h"
#include "core/search/relevance/relevance.h"

namespace yas {
class IndexSearcher {
 public:
  IndexSearcher(std::shared_ptr<IndexReader> reader);
  IndexSearcher();
  virtual ~IndexSearcher();
  void search(Query* q, MatchSet& set);
  std::shared_ptr<Query> rewrite(std::shared_ptr<Query> query);
  void set_reader(std::shared_ptr<IndexReader> reader);
  std::shared_ptr<IndexReader> get_reader();

 private:
  std::shared_ptr<IndexReader> reader_;
};

}  // namespace yas