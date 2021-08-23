#pragma once
#include <memory>
#include <vector>

#include "file.h"
#include "file_slice.h"
#include "posting_list.h"

namespace yas {
class RoaringPostingList : public PostingList {
 public:
  RoaringPostingList(File* in, uint64_t off, uint64_t length,
                     int jump_table_entry_count, long cost);
  virtual ~RoaringPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;
  static uint16_t flush(File* out, std::vector<uint32_t>& docids);
  uint32_t index();
  bool test(uint32_t target);
  bool advance_exact(uint32_t target);

 private:
  static void add_jumps(std::vector<int>& jump_tables, int from, int to,
                        int cardinality, uint64_t offset);
  static uint16_t flush_jumps(File* out, std::vector<int>& jump_tables);
  static void flush(File* out, std::vector<uint32_t>& docids, int from, int to,
                    int cardinality, uint16_t block);
  int advance_block(uint32_t target);
  int read_block_header();
  bool advance_exact_in_block(uint32_t target);

 private:
  uint32_t docid_;
  uint32_t index_;
  long cost_;
  int block_;
  uint16_t cardinality_;
  uint32_t block_start_cardinality_;
  int type_;
  int jump_table_entry_count_;
  File* in_;
  std::unique_ptr<FileSlice> jump_table_slice_;
  std::unique_ptr<FileSlice> block_slice_;
  uint64_t block_end_;
  uint16_t min_;
};

}  // namespace yas