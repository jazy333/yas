#pragma once
#include <list>
#include <queue>
#include <vector>

#include "posting_list.h"

namespace yas {
/*
 *Minimum match&weak and
 */
class WeakAndPostingList : public PostingList {
 public:
  WeakAndPostingList(std::vector<PostingList*>& pl, int minimum_match);
  virtual ~WeakAndPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;

 private:
  void add_lead(PostingList* pl);
  void update_lead();
  PostingList* add_tail(PostingList* p);
  uint32_t do_next();

 private:
 //the n-mm+1 posting list order by docid
  std::priority_queue<PostingList*, std::vector<PostingList*>,
                      posting_list_compare_with_docid>
      head_;
      // the mm-1 posting lists with least cost order by cost
  std::priority_queue<PostingList*, std::vector<PostingList*>,
                      posting_list_compare_with_cost>
      tail_;
      //matched doc
  std::list<PostingList*> lead_;
  int minimum_match_;
  uint32_t docid_;
  int matched_;
  long cost_;
};
}  // namespace yas