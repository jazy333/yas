#pragma once
#include <vector>

#include "term_reader.h"

namespace yas {
class MemoryTermReader : public TermReader {
 public:
  MemoryTermReader(std::vector<uint32_t>& posting_lists,
                   std::vector<std::vector<uint32_t>>& position_lists);
  ~MemoryTermReader() = default;
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;
  int freq() override;
  int next_postion() override;
  int doc_freq() override;

 private:
  std::vector<uint32_t> posting_lists_;
  std::vector<std::vector<uint32_t>> position_lists_;
  int index_;
  int position_index_;
};

}  // namespace yas