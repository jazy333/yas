#include "point_field_index_writer.h"

#include <gtest/gtest.h>

namespace yas {
TEST(PointFieldIndexWriter, constructor) {
  PointFieldIndexWriter<int, 2> pfiw;
  for (int start = 100; start < 300; ++start) {
    Point<int, 2> point({start, start * 3}, start);
    auto field = std::shared_ptr<PointField<int, 2>>(
        new PointField<int, 2>("point", point));
    pfiw.add(start, field);
  }

  Point<int, 2> min({1, 2000}, -1);
  Point<int, 2> max({150, 3000}, -1);
  auto pl = pfiw.get(min.bytes(), max.bytes());
  EXPECT_EQ(NDOCID, pl->next());
  delete pl;

  Point<int, 2> min1({1, 200}, -1);
  Point<int, 2> max1({150, 300}, -1);
  pl = pfiw.get(min1.bytes(), max1.bytes());
  EXPECT_EQ(100, pl->next());
  EXPECT_EQ(NDOCID, pl->next());
  delete pl;
}

TEST(PointFieldIndexWriter, flush) {
  PointFieldIndexWriter<int, 2> pfiw;
  for (int start = 100; start < 300; ++start) {
    Point<int, 2> point({start, start * 3}, start);
    auto field = std::shared_ptr<PointField<int, 2>>(
        new PointField<int, 2>("point", point));
    pfiw.add(start, field);
  }
  FieldInfo fi(155);
  IndexOption option;
  option.current_segment_no = 1;
  option.dir = "data/index";
  option.segment_prefix = "segment.";
  pfiw.flush(fi, 300, option);
}

}  // namespace yas