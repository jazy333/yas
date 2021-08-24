#include "point_field_index_writer.h"

#include <gtest/gtest.h>

namespace yas {
TEST(PointFieldIndexWriter, constructor) {
  PointFieldIndexWriter<int, 2> piw;
}
}  // namespace yas