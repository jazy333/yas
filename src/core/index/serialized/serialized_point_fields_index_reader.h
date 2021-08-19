#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "field_index_meta.h"
#include "field_index_reader.h"
#include "field_info.h"
#include "file.h"

namespace yas {
class SerializedPointFieldsIndexReader : public PointFieldsIndexReader {
 public:
  SerializedPointFieldsIndexReader(
      const std::unordered_map<std::string, FieldInfo>& field_infos,
      const std::string& meta_file, const std::string& index_file,
      const std::string& data_file);
  virtual ~SerializedPointFieldsIndexReader();
  int open() override;
  int close() override;
  PointFieldIndexReader* get_reader(
      const std::string& field_name,
      PointFieldIndexReader* init_reader) override;

 private:
  std::unordered_map<std::string, FieldInfo> field_infos_;
  std::string meta_file_;
  std::string index_file_;
  std::string data_file_;
  std::shared_ptr<File> kdm_;
  std::shared_ptr<File> kdi_;
  std::shared_ptr<File> kdd_;
  std::unordered_map<int, PointFieldMeta> kdm_infos_;
};
}  // namespace yas