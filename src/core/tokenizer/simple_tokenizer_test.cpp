#include "simple_tokenizer.h"

#include <gtest/gtest.h>

#include <string>

namespace yas {
TEST(SimpleTokenizer, contructor) {
  SimpleTokenizer st;
  EXPECT_EQ(1, st.get_interval());
}

TEST(SimpleTokenizer, set_interval) {
  SimpleTokenizer st;
  st.set_interval(3);
  EXPECT_EQ(3, st.get_interval());
}

TEST(SimpleTokenizer, get_term_iterator) {
  SimpleTokenizer st;
  st.set_interval(2);
  std::string query = "我是中国人!";
  std::vector<std::string> term_texts = {"我是", "中国", "人"};
  std::vector<int> positions = {0, 1, 2};
  std::vector<int> offsets = {2, 4, 5};
  auto ti = st.get_term_iterator(query);
  int i = 0;
  while (ti->next()) {
    Term term = ti->term();
    int pos = ti->position();
    int offset = ti->offset();
    EXPECT_EQ(term_texts[i], term.get_term());
    EXPECT_EQ(positions[i], pos);
    EXPECT_EQ(offsets[i], offset);
    i++;
  }
  EXPECT_EQ(term_texts.size(), i);

  query = "我 是中国人!";
  offsets = {3, 5, 6};
  i = 0;
  ti = st.get_term_iterator(query);
  while (ti->next()) {
    Term term = ti->term();
    int pos = ti->position();
    int offset = ti->offset();
    EXPECT_EQ(term_texts[i], term.get_term());
    EXPECT_EQ(positions[i], pos);
    EXPECT_EQ(offsets[i], offset);
    i++;
  }
  EXPECT_EQ(term_texts.size(), i);

  query = "我 是中国人Aabc!";
  term_texts = {"我是", "中国", "人a", "ab", "c"};
  offsets = {3, 5, 7, 9, 10};
  positions = {0, 1, 2, 3, 4};
  i = 0;
  ti = st.get_term_iterator(query);
  while (ti->next()) {
    Term term = ti->term();
    int pos = ti->position();
    int offset = ti->offset();
    EXPECT_EQ(term_texts[i], term.get_term());
    EXPECT_EQ(positions[i], pos);
    EXPECT_EQ(offsets[i], offset);
    i++;
  }
  EXPECT_EQ(term_texts.size(), i);

  query = "";
  ti = st.get_term_iterator(query);
  EXPECT_EQ(false, ti->next());
}

}  // namespace yas