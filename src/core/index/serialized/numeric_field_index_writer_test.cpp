#include "numeric_field_index_writer.h"

#include <gtest/gtest.h>

#include "numeric_field.h"

namespace yas {
TEST(NumericFieldIndexWriter, add) {
  NumericFieldIndexWriter nfi;
  for (int i = 0; i < 100; ++i) {
    auto field = std::shared_ptr<NumericField>(new NumericField("source", i));
    nfi.add(i + 1, field);
  }
  uint64_t value = std::numeric_limits<uint64_t>::max();
  nfi.get(1, value);
  EXPECT_EQ(0, value);
  nfi.get(2, value);
  EXPECT_EQ(1, value);
}

TEST(NumericFieldIndexWriter, flush) {
  NumericFieldIndexWriter nfi;
  for (int i = 0; i < 100; ++i) {
    auto field = std::shared_ptr<NumericField>(new NumericField("source", i));
    nfi.add(i + 1, field);
  }
  uint64_t value = std::numeric_limits<uint64_t>::max();
  nfi.get(1, value);
  EXPECT_EQ(0, value);
  nfi.get(2, value);
  EXPECT_EQ(1, value);
  FieldInfo fi(111);
  uint32_t max_doc = 1000;
  IndexOption option;
  option.current_segment_no = 1;
  option.dir = "data/index";
  option.segment_prefix = "segment.";
  nfi.flush(fi, max_doc, option);
}

}  // namespace yas