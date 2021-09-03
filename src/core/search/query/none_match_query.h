#pragma once
#include "query.h"

namespace yas {
class NoneMatchQuery : public Query {
 public:
  NoneMatchQuery();
  virtual ~NoneMatchQuery();
  std::shared_ptr<Query> rewrite() override;
  std::shared_ptr<Matcher> matcher(SubIndexReader* sub_reader) override;
};

}  // namespace yas