#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "sub_index_reader.h"

namespace yas {
class MemoryIndexReader : public SubIndexReader {
 public:
  MemoryIndexReader(
      std::shared_ptr<InvertFieldsIndexReader> invert_field_index_reader,
      std::unordered_map<std::string, PointFieldIndexReader*>*
          point_fields_index_readers,
      std::unordered_map<std::string, std::shared_ptr<FieldValueIndexReader>>*
          field_values_index_readers);
  virtual ~MemoryIndexReader() = default;
  std::shared_ptr<InvertFieldsIndexReader> invert_index_reader() override;
  std::shared_ptr<FieldValuesIndexReader> field_values_reader() override;
  std::shared_ptr<PointFieldsIndexReader> point_fields_reader() override;
  int open() override;
  int close() override;
  SegmentInfo get_segment_info() override;

 private:
  std::shared_ptr<InvertFieldsIndexReader> invert_field_index_reader_;
  std::unordered_map<std::string, PointFieldIndexReader*>*
      point_fields_index_readers_;
  std::unordered_map<std::string, std::shared_ptr<FieldValueIndexReader>>*
      field_values_index_readers_;
  SegmentInfo info_;
};

}  // namespace yas