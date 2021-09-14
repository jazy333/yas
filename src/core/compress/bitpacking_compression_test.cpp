#include "bitpacking_compression.h"

#include <gtest/gtest.h>
namespace yas {
TEST(BitPackingCompression, construcotr) {
  BitPackingCompression bpc;
  EXPECT_EQ(-1, bpc.get_max_bits());

  BitPackingCompression bpc1(8,0,1);
  EXPECT_EQ(8, bpc1.get_max_bits());
}

TEST(BitPackingCompression, compress) {
  BitPackingCompression bpc;
  EXPECT_EQ(-1, bpc.get_max_bits());
  std::vector<uint64_t> data = {1, 1, 2, 3, 4, 5};
  size_t in_size = data.size();
  size_t out_size = in_size * 8;
  uint8_t* out = new uint8_t[in_size * 8]();
  bpc.set_max_bits(3);
  bpc.compress(data.data(), in_size, out, out_size);
  EXPECT_EQ(8, out_size);
  bpc.set_max_bits(4);
  bpc.compress(data.data(), in_size, out, out_size);
  EXPECT_EQ(8, out_size);
  bpc.set_max_bits(9);
  bpc.compress(data.data(), in_size, out, out_size);
  EXPECT_EQ(8, out_size);

  bpc.set_max_bits(10);
  bpc.compress(data.data(), in_size, out, out_size);
  EXPECT_EQ(8, out_size);

  bpc.set_max_bits(11);
  bpc.compress(data.data(), in_size, out, out_size);
  EXPECT_EQ(16, out_size);
  delete[] out;
}

TEST(BitPackingCompression, decompress) {
  BitPackingCompression bpc;
  EXPECT_EQ(-1, bpc.get_max_bits());

  std::vector<uint64_t> data = {1, 1, 2, 3, 4, 5};
  size_t in_size = data.size();
  size_t out_size = in_size * 8;
  uint8_t* out = new uint8_t[in_size * 8]();

  // test for one block
  bpc.set_max_bits(3);
  bpc.compress(data.data(), in_size, out, out_size);
  EXPECT_EQ(8, out_size);
  std::vector<uint64_t> decopressed(in_size, 0);
  out_size = decopressed.size();
  bpc.decompress(out, data.size(), decopressed.data(), out_size);
  EXPECT_EQ(data.size(), out_size);
  for (int i = 0; i < data.size(); ++i) {
    EXPECT_EQ(data[i], decopressed[i]);
  }

  // test for two block
  bpc.set_max_bits(11);
  in_size = data.size();
  out_size = in_size * 8;
  bpc.compress(data.data(), in_size, out, out_size);
  out_size = decopressed.size();
  bpc.decompress(out, data.size(), decopressed.data(), out_size);
  EXPECT_EQ(data.size(), out_size);
  for (int i = 0; i < data.size(); ++i) {
    EXPECT_EQ(data[i], decopressed[i]);
  }
  delete[] out;
}


TEST(BitPackingCompression, compress1) {
  BitPackingCompression bpc(7,1250,2);
  EXPECT_EQ(-1, bpc.get_max_bits());
  std::vector<uint64_t> data = {1250, 1250, 1250, 1250, 1250, 1250,
                                1250, 1250, 1250, 1250, 1250, 1250,
                                1250, 1250, 1250, 1250, 1250, 1250,
                                1250, 1250, 1250, 1250, 1250, 1250,
                                1250, 1416, 1250, 1250, 1250, 1250,
                                1250, 1250
                                };
  size_t in_size = data.size();
  size_t out_size = in_size * 8;
  uint8_t* out = new uint8_t[in_size * 8]();
  bpc.set_max_bits(7);
  bpc.compress(data.data(), in_size, out, out_size);
  delete[] out;
}

}  // namespace yas