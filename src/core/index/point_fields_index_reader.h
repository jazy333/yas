#pragma once
#include <unordered_map>

#include "field_index_meta.h"
#include "field_info.h"
#include "file.h"
#include "point_field_index_reader.h"

namespace yas {
class PointFieldsIndexReader {
 public:
  PointFieldsIndexReader(FieldInfo* field_infos, File* kdm, File* kdi,
                         File* kdd);
  virtual ~PointFieldsIndexReader();
  int open();
  PointFieldIndexReader* get_reader(std::string field_name);

 private:
  std::unordered_map<int, PointFieldMeta> kdm_infos_;

};
}  // namespace yas