#include "skip_list.h"

#include <gtest/gtest.h>
namespace yas {
TEST(SkipList, constructor) {
  SkipList<int, int32_t> skip_list;
  EXPECT_EQ(1, skip_list.level());
}
}  // namespace yas