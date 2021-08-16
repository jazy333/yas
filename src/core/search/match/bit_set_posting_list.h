#pragma once
#include "bit_set.h"
#include "posting_list.h"

#include <memory>


namespace yas {

class BitSetPostingList : public PostingList{
 public:
  BitSetPostingList(std::unique_ptr<BitSet> bs);
  virtual ~BitSetPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;

 private:
  std::unique_ptr<BitSet> bs_;
  uint32_t docid_;
  long cost_;
};

}  // namespace yas