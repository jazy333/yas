#pragma once
#include "query.h"

namespace yas {
class AnyMatchQuery : public Query {
 public:
  AnyMatchQuery() = default;
  virtual ~AnyMatchQuery() = default;
 std::shared_ptr<Query> rewrite() override;
  std::shared_ptr<Matcher> matcher(SubIndexReader* sub_reader) override;
};
}  // namespace yas 