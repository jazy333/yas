#pragma once
#include <vector>

#include "posting_list.h"

namespace yas {
class AndPostingList : public PostingList {
 public:
  AndPostingList(std::vector<PostingList *> &pls);
  virtual ~AndPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  float score() override;
  std::string name() override;

 private:
  uint32_t do_next(uint32_t next1);
  uint32_t do_next();

 private:
  PostingList *lead1_, *lead2_;// minimum cost ,the secodary minimum
  std::vector<PostingList *> others_;
  uint32_t docid_;
};

}  // namespace yas