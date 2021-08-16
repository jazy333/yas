#include "boolean_scorer.h"

#include <gtest/gtest.h>

namespace yas {
TEST(BooleanScorer, constructor) { BooleanScorer bs(2.5); }
TEST(BooleanScorer, score) {
  BooleanScorer bs(2.5);
  EXPECT_EQ(2.5, bs.score(1, 1));
}

}  // namespace yas