#include "none_posting_list.h"

#include <gtest/gtest.h>

namespace yas {
TEST(NonePostingList, construtor) {
  NonePostingList none_pl;
  EXPECT_EQ(NDOCID,none_pl.next());
  EXPECT_EQ(NDOCID,none_pl.next());
  EXPECT_EQ(NDOCID, none_pl.docid());
  EXPECT_EQ(NDOCID, none_pl.advance(111));
  EXPECT_EQ(NDOCID, none_pl.advance(111));
}
}  // namespace yas