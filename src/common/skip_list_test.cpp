#include "skip_list.h"

#include <gtest/gtest.h>
namespace yas {
TEST(SkipList, constructor) {
  SkipList<int, int32_t> skip_list;
  EXPECT_EQ(1, skip_list.level());
}

TEST(SkipList, insert) {
  SkipList<int, int32_t> skip_list;
  skip_list.insert(111, 0);
  skip_list.insert(1, 8);
  skip_list.insert(2, 9);
  skip_list.insert(2, -2);
  skip_list.insert(4, 0);
  EXPECT_EQ(5, skip_list.size());
}

TEST(SkipList, lower_bound) {
  SkipList<int, int32_t> skip_list;
  skip_list.insert(1, 8);
  skip_list.insert(2, 9);
  skip_list.insert(2, 7);
  skip_list.insert(3, -2);
  skip_list.insert(4, 0);
  auto node = skip_list.lower_bound(4);
  EXPECT_EQ(4, node->key);
  EXPECT_EQ(0, node->value);

  node = skip_list.lower_bound(41);
  EXPECT_EQ(nullptr, node);

  node = skip_list.lower_bound(1);
  EXPECT_EQ(1, node->key);
  EXPECT_EQ(8, node->value);

  node = skip_list.lower_bound(2);
  EXPECT_EQ(2, node->key);
  EXPECT_EQ(7, node->value);
}

TEST(SkipList, uppper_bound) {
  SkipList<int, int32_t> skip_list;
  skip_list.insert(1, 8);
  skip_list.insert(2, 9);
  skip_list.insert(2, 7);
  skip_list.insert(3, -2);
  skip_list.insert(4, 0);
  auto node = skip_list.upper_bound(1);
  EXPECT_EQ(7, node->value);

  node = skip_list.upper_bound(41);
  EXPECT_EQ(nullptr, node);

  node = skip_list.upper_bound(4);
  EXPECT_EQ(nullptr, node);

  node = skip_list.upper_bound(2);
  EXPECT_EQ(3, node->key);
  EXPECT_EQ(-2, node->value);
}

TEST(SkipList, erase) {
  SkipList<int, int32_t> skip_list;
  skip_list.insert(1, 8);
  skip_list.insert(2, 9);
  skip_list.insert(2, 7);
  skip_list.insert(3, -2);
  skip_list.insert(4, 0);
  EXPECT_EQ(5, skip_list.size());
  auto node = skip_list.erase(1);
  EXPECT_EQ(4, skip_list.size());
  node = skip_list.lower_bound(1);
  EXPECT_EQ(2, node->key);
  EXPECT_EQ(7, node->value);
}

}  // namespace yas