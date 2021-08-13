#pragma once
#include <vector>

#include "posting_list.h"
namespace yas {
class SequencePostingList : public PostingList {
 public:
  SequencePostingList(std::vector<uint32_t>& docids);
  virtual ~SequencePostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  float score() override;
  std::string name() override;

 private:
  std::vector<uint32_t> docids_;
  size_t index_;
  uint32_t docid_;
};
}  // namespace yas