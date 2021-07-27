#pragma once
#include "posting_list.h"
#include "scorer.h"

namespace yas {
class NonePostingList:public PostingList,public Scorer {
 public:
  NonePostingList(/* args */);
  ~NonePostingList();
  uint32_t next();
  uint32_t advance(uint32_t target);
  uint32_t docid();
  long cost();
  std::string name();
};

}  // namespace yas