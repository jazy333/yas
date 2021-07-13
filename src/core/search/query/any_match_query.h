#pragma once
#include "query.h"

namespace yas {
class AnyMatchQuery : public Query {
 public:
  AnyMatchQuery() = default;
  ~AnyMatchQuery() = default;
  Query* rewrite();
  PostingList* posting_list(SubIndexReader* sub_reader);
  Scorer* scorer();
};
}  // namespace yas