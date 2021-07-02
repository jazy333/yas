#pragma once
#include "posting_list.h"
#include "term_reader.h"

namespace yas {
class TermPostingList : public PostingList {
 private:
  /* data */
 public:
  TermPostingList(TermReader* reader);
  ~TermPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;
};

}  // namespace yas