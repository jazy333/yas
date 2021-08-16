#pragma once
#include <memory>

#include "matcher.h"
#include "sub_index_reader.h"

namespace yas {
class Query {
 public:
  virtual Query* rewrite() = 0;
  virtual std::unique_ptr<Matcher> matcher(SubIndexReader* sub_reader) = 0;
};

}  // namespace yas