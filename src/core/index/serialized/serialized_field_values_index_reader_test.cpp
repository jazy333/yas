#include "serialized_field_values_index_reader.h"

#include <gtest/gtest.h>

namespace yas {
TEST(SerializedFieldValuesIndexReader, open) {
  SerializedFieldValuesIndexReader sfvir("data/index/segment.1.fvm",
                                         "data/index/segment.1.fvd");
  sfvir.open();
}
}  // namespace yas