#include "fixed_bit_set.h"

#include <gtest/gtest.h>
namespace yas {
TEST(FixedBitSet, FixedBitSetConstructor) {
  FixedBitSet bitset(10000);
  EXPECT_EQ(10000, bitset.size());
  EXPECT_EQ((10000 - 1) / 64 + 1, bitset.capcity());
}

TEST(FixedBitSet, set) {
  FixedBitSet bitset(10000);
  bitset.set(3);
  EXPECT_EQ(true, bitset.get(3));
  EXPECT_EQ(false, bitset.get(4));
}

TEST(FixedBitSet, next) {
  FixedBitSet bitset(1000);
  bitset.set(3);
  EXPECT_EQ(3, bitset.next(3));
  EXPECT_EQ(-1, bitset.next(4));
  EXPECT_EQ(3, bitset.next(2));
}

}  // namespace yas