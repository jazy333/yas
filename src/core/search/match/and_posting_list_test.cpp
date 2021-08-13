#include "and_posting_list.h"

#include <gtest/gtest.h>

#include <vector>

#include "sequence_posting_list.h"

namespace yas {
TEST(AndPostingList, constructor) {
  // empty posting list
  std::vector<PostingList*> pls;
  AndPostingList ap(pls);
  EXPECT_EQ(0.0, ap.score());
  EXPECT_EQ(NDOCID, ap.next());
  EXPECT_EQ(NDOCID, ap.advance(111));
  EXPECT_EQ(0, ap.cost());
  EXPECT_EQ("AndPostingList", ap.name());

  // one posting list
  std::vector<uint32_t> docids = {3, 4, 5, 6, 7};
  SequencePostingList pl1(docids);
  std::vector<PostingList*> pls1{&pl1};

  AndPostingList ap1(pls1);
  EXPECT_EQ(docids.size(), ap1.cost());
  EXPECT_EQ(0.0, ap1.score());
  EXPECT_EQ(3, ap1.next());
  EXPECT_EQ(6, ap1.advance(6));
  EXPECT_EQ(7, ap1.advance(7));
  EXPECT_EQ(NDOCID, ap1.advance(111));

  // two posting lists
  std::vector<uint32_t> docids1 = {3, 4, 5, 6, 7, 8, 11, 12, 14, 3333, 111111};
  std::vector<uint32_t> docids2 = {1, 3, 5, 6, 7, 9, 10, 13, 14, 2000};
  SequencePostingList pl3(docids1);
  SequencePostingList pl4(docids2);
  std::vector<PostingList*> pls2 = {&pl3, &pl4};
  AndPostingList ap2(pls2);
  EXPECT_EQ(docids2.size(), ap2.cost());
  EXPECT_EQ(0.0, ap2.score());
  EXPECT_EQ(3, ap2.next());
  EXPECT_EQ(6, ap2.advance(6));
  EXPECT_EQ(7, ap2.advance(7));
  EXPECT_EQ(NDOCID, ap2.advance(111));

  // others
  std::vector<uint32_t> docids3 = {3, 4, 5, 6, 7, 8, 11, 12, 14, 3333, 111111};
  std::vector<uint32_t> docids4 = {1, 3, 5, 6, 7, 9, 10, 13, 14, 2000};
  std::vector<uint32_t> docids5 = {2, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 2000};
  std::vector<uint32_t> result = {5, 6, 7, 14};
  SequencePostingList pl5(docids3);
  SequencePostingList pl6(docids4);
  SequencePostingList pl7(docids5);
  std::vector<PostingList*> pls3 = {&pl5, &pl6, &pl7};
  AndPostingList ap3(pls3);
  int count = 0;
  while (ap3.next() != NDOCID) {
    EXPECT_EQ(result[count++], ap3.docid());
    EXPECT_LE(count, result.size());
  }

  EXPECT_EQ(docids4.size(), ap3.cost());
  EXPECT_EQ(0.0, ap3.score());
}
}  // namespace yas