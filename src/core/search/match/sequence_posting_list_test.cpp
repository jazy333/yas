#include "sequence_posting_list.h"

#include <gtest/gtest.h>

namespace yas {
TEST(SequencePostingList, constructor) {
  std::vector<uint32_t> pl = {1, 3, 5, 8, 69, 9990, 11111};
  SequencePostingList sql(pl);
  EXPECT_EQ(pl.size(), sql.cost());
  EXPECT_EQ(0, sql.score());
  EXPECT_EQ(pl[0], sql.docid());
  EXPECT_EQ(pl[0], sql.next());
  EXPECT_EQ(pl[1], sql.next());
  EXPECT_EQ(pl[3], sql.advance(6));
  EXPECT_EQ(pl[6], sql.advance(11111));
  EXPECT_EQ(NDOCID, sql.next());
  EXPECT_EQ(NDOCID, sql.advance(111111));
  // empty
  pl = {};
  SequencePostingList sql1(pl);
  EXPECT_EQ(pl.size(), sql1.cost());
  EXPECT_EQ(NDOCID, sql1.docid());
  EXPECT_EQ(NDOCID, sql1.next());
  EXPECT_EQ(NDOCID, sql1.advance(11111));
  EXPECT_EQ(NDOCID, sql1.next());
}

TEST(SequencePostingList, next) {
  std::vector<uint32_t> pl = {1, 3, 5, 8, 69, 9990, 11111};
  SequencePostingList sql(pl);
  EXPECT_EQ(pl.size(), sql.cost());

  for (int i = 0; i < pl.size(); ++i) {
    EXPECT_EQ(pl[i], sql.next());
  }

  EXPECT_EQ(NDOCID, sql.next());
}

}  // namespace yas