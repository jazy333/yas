#pragma once

#include <memory>

#include "core/index/index_reader.h"
#include "core/search/match/match_set.h"
#include "core/search/query/query.h"
#include "core/search/relevance/relevance.h"
#include "common/shared_mutex.h"

namespace yas {
class IndexSearcher {
 public:
  IndexSearcher(std::shared_ptr<IndexReader> reader);
  IndexSearcher();
  virtual ~IndexSearcher();
  void search(Query* q, MatchSet& set);
  std::shared_ptr<Query> rewrite(std::shared_ptr<Query> query);
  void set_reader(std::shared_ptr<IndexReader> reader);
  //std::shared_ptr<IndexReader> get_reader();
  IndexStat get_index_stat();
  void auto_reload();

 private:
  std::shared_ptr<IndexReader> reader_;
  SharedMutex shared_mutex_;
};

}  // namespace yas