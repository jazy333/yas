#include "serialized_field_values_index_reader.h"

#include <gtest/gtest.h>

namespace yas {
TEST(SerializedFieldValuesIndexReader, open) {
  std::unordered_map<int, std::string> id2name;
  id2name[111] = "source";
  id2name[113] = "binary";
  id2name[114] = "binary_var";
  SerializedFieldValuesIndexReader sfvir("data/index1/segment.1.fvm",
                                         "data/index1/segment.1.fvd", id2name);

  sfvir.open();
  auto numeric_field = sfvir.get_reader("source");
  uint64_t value = 0;
  numeric_field->get(2, value);
  EXPECT_EQ(1, value);
  numeric_field->get(3, value);
  EXPECT_EQ(2, value);
  numeric_field->get(100, value);
  EXPECT_EQ(99, value);
  numeric_field->get(1000, value);
  EXPECT_EQ(-1, value);

  auto binary_field = sfvir.get_reader("binary");
  std::vector<uint8_t> bvalue;
  binary_field->get(1, bvalue);
  int ret = memcmp(bvalue.data(), std::to_string(100000).data(), bvalue.size());
  EXPECT_EQ(0, ret);
  EXPECT_EQ(6, bvalue.size());

  binary_field->get(99, bvalue);
  ret =
      memcmp(bvalue.data(), std::to_string(100000 + 98).data(), bvalue.size());
  EXPECT_EQ(0, ret);
  EXPECT_EQ(6, bvalue.size());

  binary_field->get(1000000, bvalue);
  EXPECT_EQ(0, bvalue.size());

  auto binary_field_var = sfvir.get_reader("binary_var");
  std::vector<uint8_t> bvalue_var;
  binary_field_var->get(1, bvalue_var);
  ret = memcmp(bvalue_var.data(), std::to_string(0).data(), bvalue_var.size());
  EXPECT_EQ(0, ret);
  EXPECT_EQ(1, bvalue_var.size());

  binary_field_var->get(99, bvalue_var);
  ret = memcmp(bvalue_var.data(), std::to_string(98).data(), bvalue_var.size());
  EXPECT_EQ(0, ret);
  EXPECT_EQ(2, bvalue_var.size());

  binary_field_var->get(1000000, bvalue_var);
  EXPECT_EQ(0, bvalue_var.size());
}
}  // namespace yas