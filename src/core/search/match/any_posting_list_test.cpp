#include "any_posting_list.h"

#include <gtest/gtest.h>

namespace yas {
TEST(AnyPostingList, constructor) {
  AnyPostingList any_pl(1111);
  EXPECT_EQ(1, any_pl.docid());
  EXPECT_EQ(2, any_pl.next());
  EXPECT_EQ(100, any_pl.advance(100));
  EXPECT_EQ(1111, any_pl.advance(1111));
  EXPECT_EQ(NDOCID, any_pl.next());
  EXPECT_EQ(NDOCID, any_pl.advance(111111));
}
}  // namespace yas