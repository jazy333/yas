#include "weak_and_posting_list.h"

#include <gtest/gtest.h>

#include "sequence_posting_list.h"

namespace yas {
TEST(WeakAndPostingList, constructor) {
  std::vector<uint32_t> docids1 = {1, 2, 3, 5, 11, 20, 30000};
  std::vector<uint32_t> docids2 = {34, 99, 2000, 200001};
  std::vector<uint32_t> docids3 = {3,  4,  5,    6,     7,    9,
                                   11, 20, 2000, 20001, 30000};
  std::shared_ptr<SequencePostingList> sql1(new SequencePostingList(docids1));
  std::shared_ptr<SequencePostingList> sql2(new SequencePostingList(docids2));
  std::shared_ptr<SequencePostingList> sql3(new SequencePostingList(docids3));
  std::vector<std::shared_ptr<PostingList>> pls1 = {sql1, sql2, sql3};
  WeakAndPostingList wap1(pls1, 2);
  EXPECT_EQ(18, wap1.cost());
  EXPECT_EQ(0.0, wap1.score());
}

TEST(WeakAndPostingList, next) {
  std::vector<uint32_t> docids1 = {1, 2, 3, 5, 11, 20, 30000};
  std::vector<uint32_t> docids2 = {3, 34, 99, 2000, 200001};
  std::vector<uint32_t> docids3 = {3,  4,  5,    6,     7,    9,
                                   11, 20, 2000, 20001, 30000};
  std::shared_ptr<SequencePostingList> sql1(new SequencePostingList(docids1));
  std::shared_ptr<SequencePostingList> sql2(new SequencePostingList(docids2));
  std::shared_ptr<SequencePostingList> sql3(new SequencePostingList(docids3));
  std::vector<std::shared_ptr<PostingList>> pls1 = {sql1, sql2, sql3};
  WeakAndPostingList wap1(pls1, 2);

  std::vector<uint32_t> result = {3, 5, 11, 20, 2000, 30000};
  int index = 0;
  while (wap1.next() != NDOCID) {
    EXPECT_LT(index, result.size());
    EXPECT_EQ(result[index++], wap1.docid());
  }
}

TEST(WeakAndPostingList, advance) {
  std::vector<uint32_t> docids1 = {1, 2, 3, 5, 11, 20, 30000};
  std::vector<uint32_t> docids2 = {3, 34, 99, 2000, 200001};
  std::vector<uint32_t> docids3 = {3,  4,  5,    6,     7,    9,
                                   11, 20, 2000, 20001, 30000};
  std::shared_ptr<SequencePostingList> sql1(new SequencePostingList(docids1));
  std::shared_ptr<SequencePostingList> sql2(new SequencePostingList(docids2));
  std::shared_ptr<SequencePostingList> sql3(new SequencePostingList(docids3));
  std::vector<std::shared_ptr<PostingList>> pls1 = {sql1, sql2, sql3};
  WeakAndPostingList wap1(pls1, 2);

  std::vector<uint32_t> result = {3, 5, 11, 20, 2000, 30000};
  EXPECT_EQ(3, wap1.advance(1));
  EXPECT_EQ(3, wap1.advance(3));
  EXPECT_EQ(5, wap1.advance(5));
  EXPECT_EQ(2000, wap1.advance(1000));
  EXPECT_EQ(2000, wap1.advance(2000));
  EXPECT_EQ(NDOCID, wap1.advance(20000000));
}

}  // namespace yas