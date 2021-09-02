#include "binary_field_index_writer.h"

#include <gtest/gtest.h>

#include "text_field.h"

namespace yas {
TEST(BinaryFieldIndexWriter, add) {
  BinaryFieldIndexWriter bfiw;
  for (int i = 0; i < 100; ++i) {
    auto field = std::shared_ptr<TextField>(
        new TextField("source", std::to_string(i + 100000)));
    bfiw.add(i + 1, field);
  }
  std::vector<uint8_t> value;
  bfiw.get(1, value);
  int ret = memcmp(value.data(), std::to_string(100000).data(), value.size());
  EXPECT_EQ(0, ret);

  bfiw.get(1000000, value);
  EXPECT_EQ(0, value.size());
}

TEST(BinaryFieldIndexWriter, flush) {
  BinaryFieldIndexWriter bfiw;
  for (int i = 0; i < 100; ++i) {
    auto field = std::shared_ptr<TextField>(
        new TextField("binary", std::to_string(i + 100000)));
    bfiw.add(i + 1, field);
  }

  FieldInfo fi(113);
  uint32_t max_doc = 1000;
  IndexOption option;
  option.current_segment_no = 1;
  option.dir = "data/index1";
  option.segment_prefix = "segment.";
  bfiw.flush(fi, max_doc, option);
}


TEST(BinaryFieldIndexWriter, flush_var) {
  BinaryFieldIndexWriter bfiw;
  for (int i = 0; i < 100; ++i) {
    auto field = std::shared_ptr<TextField>(
        new TextField("binary_var", std::to_string(i)));
    bfiw.add(i + 1, field);
  }

  FieldInfo fi(114);
  uint32_t max_doc = 1000;
  IndexOption option;
  option.current_segment_no = 1;
  option.dir = "data/index1";
  option.segment_prefix = "segment.";
  bfiw.flush(fi, max_doc, option);
}

}  // namespace yas