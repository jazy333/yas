#include "index_merger.h"

#include <gtest/gtest.h>

namespace yas {
TEST(IndexMerger,merge) {
  IndexOption option;
  option.current_segment_no = 0;
  option.auto_evict=true;
  option.mode=20;
  option.dir = "/search/workspace/ios_instant_index/index/data/service/";
  option.segment_prefix = "segment.";

  IndexOption merge_option;
  merge_option.current_segment_no = 1;
  merge_option.dir = "data/index";
  merge_option.segment_prefix = "merge.";
  IndexMerger merger(option,merge_option);
  merger.merge();
}
}  // namespace yas