#pragma once
#include "term_reader.h"
#include "term_scorer.h"

namespace yas {
class BlockTermReader : public TermReader {
 public:
  virtual uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() { return "PostingList"; }
  float score() { return 0.0f; }
  Scorer* scorer() override;
  int freq() override;
  int next_postion() override;

 private:
  DB* hdb_;
  Term* term_;
};
}  // namespace yas