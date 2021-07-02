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
  long cost() override;
  std::string name() override;
  float score() override;

 private:
  std::priority_queue<PostingList*, posting_list_compare_docid> pq_;
};

}  // namespace yas