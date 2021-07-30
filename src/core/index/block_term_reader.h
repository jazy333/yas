#pragma once
#include "term_reader.h"
#include "term_scorer.h"
#include "file_slice.h"

namespace yas {
class BlockTermReader : public TermReader {
 public:
  struct JumptTableEntry {
    uint32_t max_docid;
    uint32_t posting_list_offset;
    uint32_t position_list_offset;
  };

  struct InvertIndexMeta {
    uint32_t max_doc;
    int doc_num;
    int jump_table_entry_count;
    uint32_t posting_list_offset;
    uint32_t position_list_offset;
  };

  BlockTermReader();
  ~BlockTermReader();
  virtual uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() { return "BlockTermReader"; }
  float score() { return 0.0f; }
  Scorer* scorer() override;
  int freq() override;
  int next_postion() override;
  private:
  void next_unit(uint32_t target);

 private:
  DB* hdb_;
  Term* term_;
  uint32_t max_doc_;
  int num_docs_;
  int jump_table_entry_count_;
  int current_jump_table_entry_index_;
  JumptTableEntry* entries;
  InvertIndexMeta* meta_;
  std::string invert_index_;
  FileSlice* posting_list_slice_;
  FileSlice* position_list_slice_;

  // current jump table entry
  int current_unit_;
  JumptTableEntry* current_entry_;
  uint32_t current_unit_index_;
  uint32_t current_unit_postion_index_;
  std::vector<uint32_t> current_unit_docids_;
  std::vector<std::vector<uint32_t>> current_unit_positions_;
};
}  // namespace yas