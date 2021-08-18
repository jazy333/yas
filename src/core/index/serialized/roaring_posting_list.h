#pragma once
#include <memory>
#include <vector>

#include "file.h"
#include "file_slice.h"
#include "posting_list.h"

namespace yas {
class RoaringPostingList : public PostingList {
 public:
  RoaringPostingList(File* in, uin64_t off, uint64_t length,
                     int jump_table_entry_count, long cost);
  virtual ~RoaringPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  float score() override;
  static uint16_t flush(File* out, std::vector<uint32_t>& docids);
  static void flush(File* out, std::vector<uint32_t>& docids, int from, int to,
                    int cardinality, uint16_t block) uint32_t index();
  bool test(uint32_t target);

 private:
  uint32_t docid_;
  uint32_t index_;
  long cost_;
  uint16_t block_;
  int type_;
  int jump_table_entry_count_;
  File* in_;
  std::unique_ptr<FileSlice> jump_table_slice_;
  std::unique_ptr<FileSlice> block_slice_;
  uint64_t block_end_;
};

}  // namespace yas