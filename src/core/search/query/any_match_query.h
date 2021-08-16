#pragma once
#include "query.h"

namespace yas {
class AnyMatchQuery : public Query {
 public:
  AnyMatchQuery() = default;
  virtual ~AnyMatchQuery() = default;
  Query* rewrite() override;
  Matcher* matcher(SubIndexReader* sub_reader) override;
};
}  // namespace yas