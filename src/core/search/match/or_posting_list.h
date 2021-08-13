#pragma once
#include <queue>
#include <vector>

#include "posting_list.h"

namespace yas {

class OrPostingList : public PostingList {
 public:
  OrPostingList(std::vector<PostingList*>& pls);
  ~OrPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;

 private:
  std::priority_queue<PostingList*, std::vector<PostingList*>,
                      bool (*)(PostingList*, PostingList*)>
      pq_{posting_list_compare_with_docid};
};

}  // namespace yas