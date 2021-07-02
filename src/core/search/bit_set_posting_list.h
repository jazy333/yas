#pragma once
#include "bitset.h"
#include "posting_list.h"

namespace yas {

class BitSetPostingList : public PostingList {
 private:
  BitSet* bs_;
  uint32_t docid_;
  long cost_;

 public:
  BitSetPostingList(BitSet*);
  ~BitSetPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;
};

}  // namespace yas