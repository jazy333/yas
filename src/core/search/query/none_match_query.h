#pragma once
#include "query.h"

namespace yas {
class NoneMatchQuery : public Query {
 public:
  NoneMatchQuery();
  virtual ~NoneMatchQuery();
  Query* rewrite() override;
  std::unique_ptr<Matcher> matcher(SubIndexReader* sub_reader) override;
};

}  // namespace yas