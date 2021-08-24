#pragma once

#include <string>
#include <unordered_map>

#include "field_index_meta.h"
#include "field_index_reader.h"
#include "file.h"

namespace yas {
class SerializedFieldValuesIndexReader : public FieldValuesIndexReader {
 public:
  SerializedFieldValuesIndexReader(
      const std::string& meta_file, const std::string& data_file,
      std::unordered_map<int, std::string>& field_id_name);
  virtual ~SerializedFieldValuesIndexReader();
  int open() override;
  int close() override;
  std::shared_ptr<FieldValueIndexReader> get_reader(
      const std::string& field_name) override;

 private:
  void read_numeric(const std::string& field_name, int field_id, int8_t type);
  void read_binary(const std::string& field_name, int field_id, int8_t type);

 private:
  std::string meta_file_;
  std::string data_file_;
  File* field_values_meta_;
  File* field_values_data_;
  std::unordered_map<int, std::string> field_id_name_;
  std::unordered_map<std::string, FieldIndexMeta*> field_index_metas_;
};

}  // namespace yas