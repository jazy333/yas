#include <vector>

#include "file.h"
#include "file_slice.h"
#include "posting_list.h"

namespace yas {
class RoaringPostingList {
 public:
  RoaringPostingList(File* in,loff_t off,loff_t length,int jump_table_entry_count,long cost);
  ~RoaringPostingList();
  uint32_t next() override;
  uint32_t advance(uint32_t target) override;
  uint32_t docid() override;
  long cost() override;
  std::string name() override;
  static uint16_t make(std::vector<uint32_t>& docids, File* out);
  uint32_t index();
  bool test(uint32_t target);

 private:
  uint32_t docid_;
  uint32_t index_;
  long cost_;
  uint16_t block_;
  int type_;
  int jump_table_entry_count_;
  File* in_;
  FileSlice* jump_table_slice_;
  FileSlice*  block_slice_;
  loff_t block_end_;
};

}  // namespace yas