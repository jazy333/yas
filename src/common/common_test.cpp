#include "common.h"

#include <gtest/gtest.h>
namespace yas {
TEST(uint2uchar, less16) {
  uint8_t encode1 = uint2uchar(15);
  uint8_t encode2 = uint2uchar(4);
  EXPECT_EQ(15, encode1);
  EXPECT_EQ(4, encode2);
  EXPECT_GT(encode1, encode2);
}

TEST(uint2uchar, ge16) {
  uint8_t encode1 = uint2uchar(21);
  uint8_t encode2 = uint2uchar(22);
  uint8_t encode3 = uint2uchar(3333);
  uint8_t encode4 = uint2uchar(15);
  EXPECT_GE(encode2, encode1);
  EXPECT_GE(encode3, encode1);
  EXPECT_GE(encode3, encode4);
}

TEST(uchar2uint, ge16) {
  uint8_t encode1 = uint2uchar(21);
  uint8_t encode2 = uint2uchar(22);
  uint8_t encode3 = uint2uchar(3333);
  uint8_t encode4 = uint2uchar(15);
  EXPECT_GE(encode2, encode1);
  EXPECT_GE(encode3, encode1);

  uint32_t decode1 = uchar2uint(encode1);
  uint32_t decode2 = uchar2uint(encode2);
  uint32_t decode3 = uchar2uint(encode3);
  uint32_t decode4 = uchar2uint(encode4);
  EXPECT_GE(decode2, decode1);
  EXPECT_GE(decode3, decode1);
  EXPECT_GE(decode4, decode4);
}
}  // namespace yas