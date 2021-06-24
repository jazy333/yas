#pragma once
#include "posting_list.h"

namespace yas {
class Query {
 private:
  /* data */
 public:
  Query(/* args */) = default;
  virtual ~Query() = default;
  virtual Query* rewrite(Query*) = 0;
  virtual PostingList* docid_list()=0;
};

}  // namespace yas