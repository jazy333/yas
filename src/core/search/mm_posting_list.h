#pragma once
#include <list>
#include <queue>
#include <vector>

#include "posting_list.h"

namespace yas {
/*
 *Minimum match
 */
class MMPostingList : public PostingList {
 public:
  MMPostingList(std::vector<PostingList*>& pl, int minimum_match);
  ~MMPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;

 private:
  void add_lead(PostingList* pl);
  void update_lead();
  uint32_t do_next();
  PostingList* add_tail();
  std::priority_queue<PostingList*, posting_list_compare_docid> head_;
  std::priority_queue<PostingList*, posting_list_compare_cost> tail_;
  std::list<PostingList*> lead_;
  int minimum_match_;
  uint32_t docid_;
  int matched_;
};
}  // namespace yas