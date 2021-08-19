#include "bit_set_posting_list.h"

#include <gtest/gtest.h>

#include "fixed_bit_set.h"

namespace yas {

TEST(BitSetPostingList, constructor) {
  std::unique_ptr<FixedBitSet> fbs(new FixedBitSet(1000));
  fbs->set(1);
  fbs->set(111);
  fbs->set(133);
  fbs->set(988);
  BitSetPostingList bspl(std::move(fbs));
}

TEST(BitSetPostingList, next) {
   std::unique_ptr<FixedBitSet> fbs(new FixedBitSet(1000));
  fbs->set(1);
  fbs->set(111);
  fbs->set(133);
  fbs->set(988);
  BitSetPostingList bspl(std::move(fbs));
  std::vector<uint32_t> result{1, 111, 133, 988};
  int index = 0;
  while (bspl.next() != NDOCID) {
    EXPECT_EQ(result[index], bspl.docid());
  }
  EXPECT_EQ(NDOCID, bspl.docid());
  EXPECT_EQ(NDOCID, bspl.next());
}

TEST(BitSetPostingList, advance) {
   std::unique_ptr<FixedBitSet> fbs(new FixedBitSet(1000));
  fbs->set(1);
  fbs->set(111);
  fbs->set(133);
  fbs->set(988);
  BitSetPostingList bspl(std::move(fbs));
  EXPECT_EQ(1, bspl.advance(1));
  EXPECT_EQ(111, bspl.advance(3));

  EXPECT_EQ(1000, bspl.advance(1000));
  EXPECT_EQ(NDOCID, bspl.advance(31111));
}

}  // namespace yas