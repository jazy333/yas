#include "serialized_point_fields_index_reader.h"

#include <gtest/gtest.h>

#include "point_field.h"

namespace yas {
TEST(SerializedPointFieldsIndexReader, open_and_close) {
  FieldInfo fi(155);
  std::unordered_map<std::string, FieldInfo> field_infos;
  field_infos["point"] = fi;
  std::string kdm = "data/index1/segment.1.kdm";
  std::string kdi = "data/index1/segment.1.kdi";
  std::string kdd = "data/index1/segment.1.kdd";
  SerializedPointFieldsIndexReader spfir(field_infos, kdm, kdi, kdd);
  spfir.open();
  spfir.close();
}

TEST(SerializedPointFieldsIndexReader, get_reader) {
  FieldInfo fi(155);
  std::unordered_map<std::string, FieldInfo> field_infos;
  field_infos["point"] = fi;
  std::string kdm = "data/index1/segment.1.kdm";
  std::string kdi = "data/index1/segment.1.kdi";
  std::string kdd = "data/index1/segment.1.kdd";
  SerializedPointFieldsIndexReader spfir(field_infos, kdm, kdi, kdd);
  spfir.open();
  PointField<int, 2> field;
  auto field_index_reader = field.make_serialized_field_index_reader();
  spfir.get_reader("point", field_index_reader);

  Point<int, 2> min({1, 2000}, -1);
  Point<int, 2> max({150, 3000}, -1);
  auto pl = field_index_reader->get(min.bytes(), max.bytes());
  EXPECT_EQ(NDOCID, pl->next());
  

  Point<int, 2> min1({1, 200}, -1);
  Point<int, 2> max1({150, 300}, -1);
  pl = field_index_reader->get(min1.bytes(), max1.bytes());
  EXPECT_EQ(100, pl->next());
  EXPECT_EQ(NDOCID, pl->next());

  delete field_index_reader;
}

}  // namespace yas