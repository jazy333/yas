#include "not_posting_list.h"

#include <gtest/gtest.h>

#include <vector>

#include "sequence_posting_list.h"

namespace yas {
TEST(NotPostingList, constructor) {
  std::vector<uint32_t> docids1{1, 2, 3, 4, 6};
  std::vector<uint32_t> docids2{3, 4, 6, 9};

  std::shared_ptr<PostingList> sq1(new SequencePostingList(docids1));
  std::shared_ptr<PostingList> sq2(new SequencePostingList(docids2));

  NotPostingList npl(sq1, sq2);

  EXPECT_EQ(5, npl.cost());
}

TEST(NotPostingList, next) {
  std::vector<uint32_t> docids1{1, 2, 3, 4, 6};
  std::vector<uint32_t> docids2{3, 4, 6, 9};

  std::shared_ptr<PostingList> sq1(new SequencePostingList(docids1));
  std::shared_ptr<PostingList> sq2(new SequencePostingList(docids2));

  NotPostingList np1(sq1, sq2);

  EXPECT_EQ(1, np1.next());
  EXPECT_EQ(2, np1.next());
  EXPECT_EQ(NDOCID, np1.next());
}

TEST(NotPostingList, advance) {
  std::vector<uint32_t> docids1{1, 2, 3, 4, 6};
  std::vector<uint32_t> docids2{3, 4, 6, 9};

  std::shared_ptr<PostingList> sq1(new SequencePostingList(docids1));
  std::shared_ptr<PostingList> sq2(new SequencePostingList(docids2));

  NotPostingList np1(sq1, sq2);

  EXPECT_EQ(1, sq1->advance(1));
  EXPECT_EQ(2, sq1->advance(2));
  EXPECT_EQ(NDOCID, np1.advance(3));
  EXPECT_EQ(NDOCID, np1.advance(4));
  EXPECT_EQ(NDOCID, np1.advance(1000));
}
}  // namespace yas