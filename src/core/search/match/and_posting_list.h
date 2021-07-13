#pragma once

#include "posting_list.h"

#include <vector>

namespace yas {
class AndPostingList : public PostingList {
 private:
  /* data */
  PostingList *lead1_, *lead2_;
  std::vector<PostingList *> others_;
  uint32_t do_next(uint32_t next1);
  uint32_t do_next();

 public:
  AndPostingList(/* args */);
  ~AndPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
 
};

}  // namespace yas