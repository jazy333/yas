#include "term.h"

#include <gtest/gtest.h>

namespace yas {
TEST(Term, DefaultConstructor) {
  Term term;
  EXPECT_EQ("", term.get_term());
  EXPECT_EQ("", term.get_field());
}

TEST(Term, ConstructorFromString) {
  Term term("testterm", "testfield");
  EXPECT_EQ("testterm", term.get_term());
  EXPECT_EQ("testfield", term.get_field());
}

TEST(Term, set) {
  Term term("testterm", "testfield");
  term.set_term("testterm1");
  term.set_field("testfield1");
  EXPECT_EQ("testterm1", term.get_term());
  EXPECT_EQ("testfield1", term.get_field());
}

}  // namespace yas
