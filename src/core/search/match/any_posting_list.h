#pragma once
#include "posting_list.h"
#include "scorer.h"

namespace yas {
class AnyPostingList : public PostingList,public Scorer {
 public:
  AnyPostingList(uint32_t max_docid);
  ~AnyPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  
 private:
  uint32_t docid_;
  uint32_t max_docid_;
};
}  // namespace yas