#pragma once

#include "term_reader.h"
#include <vector>

namespace yas {
class MemoryInvertFieldIndexReader : public TermReader {
 public:
  MemoryInvertFieldIndexReader(std::string,
                               std::vector<uint32_t>* posting_lists,
                               std::vector<std::vector<int>>* postion_lists);
  ~MemoryInvertFieldIndexReader() = default;
  virtual uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() { return "MemoryInvertFieldIndexReader"; }
  float score() { return 0.0f; }
  Scorer* scorer() override;
  int freq() override;
  int next_postion() override;

 private:
  std::vector<uint32_t> posting_lists_;
  std::vector<std::vector<int>> position_lists_;
  int index_;
  int postion_index_;
};

}  // namespace yas