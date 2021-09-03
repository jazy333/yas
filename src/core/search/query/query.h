#pragma once
#include <memory>

#include "matcher.h"
#include "sub_index_reader.h"

namespace yas {
class Query {
 public:
  virtual ~Query() = default;
  virtual std::shared_ptr<Query> rewrite() = 0;
  virtual std::shared_ptr<Matcher> matcher(SubIndexReader* sub_reader) = 0;
};

}  // namespace yas