#pragma once
#include "query.h"

namespace yas {
class NoneMatchQuery : public Query {
 private:
  /* data */
 public:
  NoneMatchQuery(/* args */);
  ~NoneMatchQuery();
  Query* rewrite();
  PostingList* posting_list(SubIndexReader* sub_reader);
  Scorer* scorer() ;
};

}  // namespace yas