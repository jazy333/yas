#include "and_posting_list.h"

#include <gtest/gtest.h>

#include <vector>

namespace yas {
TEST(AndPostingList, constructor) {
  // empty posting list
  std::vector<PostingList*> pls;
  AndPostingList ap(pls);
  EXPECT_EQ(0.0, ap.score());
  EXPECT_EQ(NDOCID, ap.next());
  EXPECT_EQ(NDOCID, ap.advance(111));

  //one posting list
  //two posting lists
  //others
}
}  // namespace yas