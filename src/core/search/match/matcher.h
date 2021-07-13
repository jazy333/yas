#pragma once
#include "match_set.h"
#include "posting_list.h"
#include "sub_index_reader.h"

namespace yas {
class Matcher {
 public:
  virtual PostingList* posting_list() = 0;
  virtual Scorer* scorer() = 0;
};
}  // namespace yas