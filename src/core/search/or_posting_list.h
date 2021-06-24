#pragma once

#include <queue>

#include "posting_list.h"

namespace yas {
    
class OrPostingList : public PostingList {
 public:
  OrPostingList(/* args */);
  ~OrPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  virtual long cost() override;

 private:
 bool posting_list_compare(PostingList* l,PostingList *r);
 std::priority_queue<PostingList*,posting_list_compare> pq_;
};

}  // namespace yas