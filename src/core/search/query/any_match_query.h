#pragma once
#include "query.h"

namespace yas {
class AnyMatchQuery : public Query {
 public:
  AnyMatchQuery() = default;
  virtual ~AnyMatchQuery() = default;
  Query* rewrite() override;
  std::unique_ptr<Matcher> matcher(SubIndexReader* sub_reader) override;
};
}  // namespace yas