#include "index_reader.h"

#include <gtest/gtest.h>

namespace yas {
TEST(IndexReader, open) {
  IndexOption option;
  option.current_segment_no = 1;
  option.dir = "data/index";
  option.segment_prefix = "segment.";
  IndexReader reader(option);
  reader.open();
  reader.close();
}
}  // namespace yas