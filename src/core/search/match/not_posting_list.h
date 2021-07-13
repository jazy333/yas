#pragma once
#include "posting_list.h"

namespace yas {
class NotPostingList : public PostingList {
 private:
  PostingList *pl_, *excl_;
  uint32_t do_next(uint32_t next);

 public:
  NotPostingList(PostingList* pl, PostingList* excl);
  ~NotPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;
};

}  // namespace yas