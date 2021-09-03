#include "index_merger.h"

#include <gtest/gtest.h>

namespace yas {
TEST(IndexMerger,merge) {
  IndexOption option;
  option.current_segment_no = 3;
  option.dir = "data/index";
  option.segment_prefix = "segment.";
  IndexMerger merger(option);
  merger.merge();
}
}  // namespace yas