#pragma once
#include <vector>

#include "posting_list.h"

namespace yas {
class AndPostingList : public PostingList {
 private:
  /* data */
  PostingList *lead1, *lead2;
  std::vector<PostingList*> others;
  uint32_t do_next(uint32_t next1);
  uint32_t do_next();

 public:
  AndPostingList(/* args */);
  ~AndPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  virtual long cost() override;
};

}  // namespace yas