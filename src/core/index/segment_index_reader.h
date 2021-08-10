#pragma once
#include "segment_files.h"
#include "sub_index_reader.h"

namespace yas {
class SegmentIndexReader : public SubIndexReader {
 public:
  SegmentIndexReader(SegmentFiles files);
  virtual ~SegmentIndexReader();
  InvertFieldsIndexReader* invert_index_reader() override;
  FieldValuesIndexReader* field_values_reader() override;
  PointFieldsIndexReader* point_fields_reader() override;
  int open() override;
  int close() override;

 private:
  SegmentFiles files_;
  PointFieldsIndexReader* point_fields_index_reader_;
  FieldValuesReader* field_values_index_reader_;
  InvertFieldsIndexReader* invert_fields_index_readesr_;
};
}  // namespace yas