#pragma once
#include "match_set.h"
#include "query.h"
#include "index_reader.h"
#include "relevance.h"

namespace yas {
class IndexSearcher {
 private:
  IndexReader* reader_;
  Relevance * rel;
 public:
  IndexSearcher(IndexReader* reader);
  virtual ~IndexSearcher();
  void search(Query* q, MatchSet& set);
};

}  // namespace yas