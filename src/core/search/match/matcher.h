#pragma once
#include "posting_list.h"
#include "scorer.h"

namespace yas {
class Matcher {
 public:
  virtual ~Matcher() = default;
  virtual std::shared_ptr<PostingList> posting_list() = 0;
  virtual std::shared_ptr<Scorer> scorer() = 0;
};
}  // namespace yas