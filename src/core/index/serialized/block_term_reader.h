#pragma once
#include "db.h"
#include "file_slice.h"
#include "term.h"
#include "term_reader.h"
#include "term_scorer.h"

namespace yas {

class BlockTermReader : public TermReader {
 public:
  struct JumpTableEntry {
    uint32_t max_docid;
    uint64_t posting_list_offset;
    uint64_t position_list_offset;
  } __attribute__((packed));

  struct InvertIndexMeta {
    uint32_t max_doc;
    uint64_t doc_num;
    uint32_t last_unit_posting_list_compress_size;
    uint32_t last_unit_position_lens_compress_size;
    uint64_t posting_list_start;
    uint64_t position_list_start;
    int jump_table_entry_count;
  } __attribute__((packed));

  struct compare_with_jump_table_entry {
    bool operator()(JumpTableEntry a, uint32_t target) {
      return a.max_docid < target;
    }
  };

  BlockTermReader(DB* db, Term* term);
  BlockTermReader(const std::string& invert_index);
  virtual ~BlockTermReader();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;
  int freq() override;
  int next_postion() override;
  int doc_freq() override;
  void set_scorer(Scorer* scorer) override;
  std::vector<uint32_t> positions();

 private:
  void next_unit(uint32_t target);
  int read_data();
  void reset_current_unit();

 private:
  DB* db_;
  Term* term_;
  int num_docs_;
  uint32_t docid_;
  int current_jump_table_entry_index_;
  JumpTableEntry* entries_;
  InvertIndexMeta* meta_;
  std::string invert_index_;

  // current jump table entry
  JumpTableEntry* current_entry_;
  int current_unit_index_;
  uint32_t current_unit_max_docid_;
  uint32_t current_unit_position_index_;
  std::vector<uint32_t> current_unit_docids_;
  size_t unit_size_;
  std::vector<std::vector<uint32_t>> current_unit_positions_;
  Scorer* scorer_;
};
}  // namespace yas