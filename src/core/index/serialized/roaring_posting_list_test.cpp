#include "roaring_posting_list.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <ctime>
#include <random>
#include <unordered_set>
#include <vector>

#include "mmap_file.h"

namespace yas {

void generate_docids(std::vector<uint32_t>& docids) {
  std::default_random_engine ran(time(nullptr));
  std::uniform_int_distribution<uint16_t> block_and_num_dis(0, 65535);

  uint16_t block_num = 3;
  for (uint16_t i = 0; i < block_num; ++i) {
    uint16_t docid_num_in_block = block_and_num_dis(ran);
    std::unordered_set<uint16_t> uniq;
    for (uint16_t j = 0; j < docid_num_in_block; ++j) {
      uint16_t docid_in_block = block_and_num_dis(ran);
      if (uniq.count(docid_in_block) == 1)
        continue;
      else {
        uint32_t docid = i << 16 | docid_in_block;
        docids.push_back(docid);
        uniq.insert(docid_in_block);
      }
    }
  }

  sort(docids.begin(), docids.end());
}

TEST(RoaringPostingList, flush) {
  File* out = new MMapFile();
  out->open("data/index/test.rpl", true);
  std::vector<uint32_t> docids;
  // generate_docids(docids);

  // parse
  for (int i = 1; i <= 300; ++i) {
    docids.push_back(i);
  }

  // full
  for (int i = 65536; i < 65536 * 3; ++i) {
    docids.push_back(i);
  }

  // dense
  for (int i = 65536 * 3 + 1000; i < 65536 * 4 - 1000; ++i) {
    docids.push_back(i);
  }

  uint16_t jump_table_entry_count = RoaringPostingList::flush(out, docids);
  EXPECT_EQ(4, jump_table_entry_count);
}

TEST(RoaringPostingList, constructor) {
  File* in = new MMapFile();
  in->open("data/index/test.rpl", false);
  size_t length = in->size();
  RoaringPostingList rpl(in, 0, length, 4, 65536 * 4);
  bool ret = rpl.advance_exact(2);
  EXPECT_EQ(true, ret);
  EXPECT_EQ(1, rpl.index());

  ret = rpl.advance_exact(300);
  EXPECT_EQ(true, ret);
  EXPECT_EQ(299, rpl.index());

  ret = rpl.advance_exact(301);
  EXPECT_EQ(false, ret);

  ret = rpl.advance_exact(65536 + 199);
  EXPECT_EQ(true, ret);
  EXPECT_EQ(300 + 199, rpl.index());

  ret = rpl.advance_exact(65536 * 3 + 1299);
  EXPECT_EQ(true, ret);
  EXPECT_EQ(300 + 2 * 65536 + 299, rpl.index());

  ret = rpl.advance_exact(65536 * 5);
  EXPECT_EQ(false, ret);

  ret = rpl.advance_exact(65536 * 3 + 299);
  EXPECT_EQ(false, ret);
}

}  // namespace yas