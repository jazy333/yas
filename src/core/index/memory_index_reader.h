#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "sub_index_reader.h"

namespace yas {
class MemoryIndexReader : public SubIndexReader {
  MemoryIndexReader(
      std::shared_ptr<InvertFieldsIndexWriter>* invert_field_index_writer,
      std::unordered_map<std::string, std::unique_ptr<FieldIndexWriter>>*
          point_fields_index_writers,
      std::unordered_map<std::string, std::unique_ptr<FieldIndexWriter>>*
          field_values_index_writers);
  std::shared_ptr<InvertFieldsIndexReader> invert_index_reader() override;
  std::shared_ptr<FieldValuesIndexReader> field_values_reader() override;
  std::shared_ptr<PointFieldsIndexReader> point_fields_reader() override;
  int open() override;
  int close() override;

 private:
  std::shared_ptr<InvertFieldsIndexWriter> invert_field_index_writer_;
  std::unordered_map<std::string, std::unique_ptr<FieldIndexWriter>>*
      point_fields_index_writers_;
  std::unordered_map<std::string, std::unique_ptr<FieldIndexWriter>>*
      field_values_index_writers_;
};

}  // namespace yas