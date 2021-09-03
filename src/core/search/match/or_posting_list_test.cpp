#include "or_posting_list.h"

#include <gtest/gtest.h>

#include "sequence_posting_list.h"

namespace yas {
TEST(OrPostingList, constructor) {
  std::vector<uint32_t> docids1 = {1, 2, 3, 5, 11, 20, 30000};
  std::vector<uint32_t> docids2 = {34, 99, 2000, 200001};
  std::vector<uint32_t> docids3 = {3,  4,  5,    6,     7,    9,
                                   11, 20, 2000, 20001, 30000};
  std::shared_ptr<SequencePostingList> sql1(new SequencePostingList(docids1));
  std::shared_ptr<SequencePostingList> sql2(new SequencePostingList(docids2));
  std::shared_ptr<SequencePostingList> sql3(new SequencePostingList(docids3));
  std::vector<std::shared_ptr<PostingList>> pls1 = {sql1, sql2, sql3};
  OrPostingList opl(pls1);
}

TEST(OrPostingList, next) {
  std::vector<uint32_t> docids1 = {1, 2, 3, 5, 11, 20, 30000};
  std::vector<uint32_t> docids2 = {34, 99, 2000, 200001};
  std::vector<uint32_t> docids3 = {3,  4,  5,    6,     7,    9,
                                   11, 20, 2000, 20001, 30000};
  std::shared_ptr<SequencePostingList> sql1(new SequencePostingList(docids1));
  std::shared_ptr<SequencePostingList> sql2(new SequencePostingList(docids2));
  std::shared_ptr<SequencePostingList> sql3(new SequencePostingList(docids3));
  std::vector<std::shared_ptr<PostingList>> pls1 = {sql1, sql2, sql3};
  OrPostingList opl(pls1);
  std::vector<uint32_t> result = {1,  2,  3,  4,  5,    6,     7,     9,
                                  11, 20, 34, 99, 2000, 20001, 30000, 200001};
  int index = 0;
  while (opl.next() != NDOCID) {
    EXPECT_EQ(result[index++], opl.docid());
  }

  EXPECT_EQ(index, result.size());
}

TEST(OrPostingList, advance) {
  std::vector<uint32_t> docids1 = {1, 2, 3, 5, 11, 20, 30000};
  std::vector<uint32_t> docids2 = {34, 99, 2000, 200001};
  std::vector<uint32_t> docids3 = {3,  4,  5,    6,     7,    9,
                                   11, 20, 2000, 20001, 30000};
  std::shared_ptr<SequencePostingList> sql1(new SequencePostingList(docids1));
  std::shared_ptr<SequencePostingList> sql2(new SequencePostingList(docids2));
  std::shared_ptr<SequencePostingList> sql3(new SequencePostingList(docids3));
  std::vector<std::shared_ptr<PostingList>> pls1 = {sql1, sql2, sql3};
  OrPostingList opl(pls1);
  EXPECT_EQ(1, opl.advance(1));
  EXPECT_EQ(9, opl.advance(8));
  EXPECT_EQ(200001, opl.advance(30001));
  EXPECT_EQ(NDOCID, opl.advance(30001000));
}
}  // namespace yas