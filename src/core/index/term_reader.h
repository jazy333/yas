#pragma once
#include "posting_list.h"

namespace yas {
class TermReader : public PostingList {
 public:
  virtual int freq() = 0;
  virtual int next_postion() = 0;
};
}  // namespace yas