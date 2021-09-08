#include "index_reader.h"

#include <gtest/gtest.h>

namespace yas {
TEST(IndexReader, open) {
  IndexOption option;
  option.current_segment_no = 0;
  option.dir = "/search/workspace/ios_instant_index/index/data/service/merge/";
  option.segment_prefix = "merge.";
  IndexReader reader(option);
  reader.open();
  reader.close();
}
}  // namespace yas