#include "delimeter_tokenizer.h"

#include <gtest/gtest.h>

namespace yas {
TEST(DelimeterTokenizer, get_term_iterator) {
  DelimeterTokenizer deli;
  std::string query = "我是 中国 人";
  std::vector<std::string> term_texts = {"我是", "中国", "人"};
  std::vector<int> positions = {0, 1, 2};
  std::vector<int> offsets = {2, 4, 5};
  auto ti = deli.get_term_iterator(query);
  int i = 0;
  while (ti->next()) {
    Term term = ti->term();
    int pos = ti->position();
    int offset = ti->offset();
    EXPECT_EQ(term_texts[i], term.get_term());
    EXPECT_EQ(positions[i], pos);
    i++;
  }
  EXPECT_EQ(term_texts.size(), i);
}
}  // namespace yas