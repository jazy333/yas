#include "boolean_relevance.h"

#include <gtest/gtest.h>

namespace yas {
TEST(BooleanRelevance, constructor) {
  BooleanRelevance br;
  IndexStat is;
  Term term;
  TermStat ts(term, 10000, 200);
  RelevanceScorer* scorer = br.scorer(3.8, is, ts);
  EXPECT_FLOAT_EQ(3.8, scorer->score(1, 1));
}

}  // namespace yas