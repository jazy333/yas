#pragma once
#include "posting_list.h"
#include "scorer.h"

namespace yas {
class Matcher {
 public:
  virtual PostingList* posting_list() = 0;
  virtual Scorer* scorer() = 0;
};
}  // namespace yas