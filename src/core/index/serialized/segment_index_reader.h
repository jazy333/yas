#pragma once
#include <memory>
#include <unordered_map>

#include "segment_files.h"
#include "sub_index_reader.h"

namespace yas {
class SegmentIndexReader : public SubIndexReader {
 public:
  SegmentIndexReader(
      SegmentFiles files,
      const std::unordered_map<std::string, FieldInfo>& field_infos);
  virtual ~SegmentIndexReader();
  std::shared_ptr<InvertFieldsIndexReader> invert_index_reader() override;
  std::shared_ptr<FieldValuesReader>  field_values_reader() override;
  std::shared_ptr<PointFieldsIndexReader> point_fields_reader() override;
  int open() override;
  int close() override;

 private:
  SegmentFiles files_;
  std::unordered_map<std::string, FieldInfo> field_infos_;
  std::shared_ptr<PointFieldsIndexReader> point_fields_index_reader_;
  std::shared_ptr<FieldValuesReader> field_values_index_reader_;
  std::shared_ptr<InvertFieldsIndexReader> invert_fields_index_readesr_;
};
}  // namespace yas