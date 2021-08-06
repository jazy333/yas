#pragma once

#include <string>
#include <unordered_map>

#include "field_index_meta.h"
#include "field_index_reader.h"
#include "file.h"
#include "numeric_field_index_reader.h"

namespace yas {
class FieldValuesReader {
 public:
  FieldValuesReader(File* meta, File* data);
  virtual ~FieldValuesReader();
  void read();
  FieldIndexReader* get(const std::string& field_name);

 private:
  void read_numeric(const std::string& field_name);
  void read_binary(const std::string& field_name);

 private:
  File* field_values_meta_;
  File* field_values_data_;
  std::unordered_map<std::string, FieldIndexMeta*> field_index_metas_;
};

}  // namespace yas