#include "index_merger.h"

#include <gtest/gtest.h>

namespace yas {
TEST(IndexMerger,merge) {
  IndexOption option;
  option.current_segment_no = 3;
  option.dir = "data/index";
  option.segment_prefix = "segment.";

  IndexOption merge_option;
  option.current_segment_no = 1;
  option.dir = "data/index";
  option.segment_prefix = "merge.";
  IndexMerger merger(option,merge_option);
  merger.merge();
}
}  // namespace yas