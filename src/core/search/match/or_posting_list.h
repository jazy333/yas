#pragma once
#include <memory>
#include <queue>
#include <vector>

#include "posting_list.h"

namespace yas {

class OrPostingList : public PostingList {
 public:
  OrPostingList(std::vector<std::shared_ptr<PostingList>>& pls);
  virtual ~OrPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;
  std::unordered_map<std::string, int> hit() override;

 private:
  std::priority_queue<std::shared_ptr<PostingList>,
                      std::vector<std::shared_ptr<PostingList>>,
                      posting_list_compare_with_docid>
      pq_;
  long cost_;
  uint32_t docid_;
};

}  // namespace yas