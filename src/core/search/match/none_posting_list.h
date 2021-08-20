#pragma once
#include "posting_list.h"

namespace yas {
class NonePostingList : public PostingList {
 public:
  NonePostingList();
  virtual ~NonePostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  float score() override;
  std::string name() override;
};

}  // namespace yas