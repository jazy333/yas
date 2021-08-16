#pragma once

#include <string>
#include <unordered_map>

#include "field_index_meta.h"
#include "field_index_reader.h"
#include "file.h"
#include "numeric_field_index_reader.h"

namespace yas {
class SerializedFieldValuesReader : public FieldValuesIndexReader {
 public:
  SerializedFieldValuesReader(const std::string& meta_file,
                              const std::string& data_file);
  virtual ~SerializedFieldValuesReader();
  int open() override;
  int close() override;
  FieldValueIndexReader* get(const std::string& field_name) override;

 private:
  void read_numeric(const std::string& field_name);
  void read_binary(const std::string& field_name);

 private:
  std::string meta_file_;
  std::string data_file_;
  File* field_values_meta_;
  File* field_values_data_;
  std::unordered_map<std::string, FieldIndexMeta*> field_index_metas_;
};

}  // namespace yas