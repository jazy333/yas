#include "segment_index_reader.h"
#include "serialized_field_values_reader.h"
#include "serialized_invert_fields_index_reader.h"
#include "serialized_point_fields_index_reader.h"

namespace yas {
SegmentIndexReader::SegmentIndexReader(
    SegmentFiles files,
    const std::unordered_map<std::string, FieldInfo>& field_infos)
    : files_(files), field_infos_(field_infos) {}

SegmentIndexReader::~SegmentIndexReader() {}

std::shared_ptr<InvertFieldsIndexReader>
iSegmentIndexReader::invert_index_reader() {
  return invert_fields_index_reader_;
}

std::shared_ptr<FieldValuesReader> SegmentIndexReader::field_values_reader() {
  return field_values_index_reader_;
}

std::shared_ptr<PointFieldsIndexReader>
SegmentIndexReader::point_fields_reader() {
  return point_fields_index_reader_;
}

int SegmentIndexReader::open() {
  point_fields_index_reader_ = new SerializedPointFieldsIndexReader(
      field_infos_, files_.kdm, files_.kdi, files_.kdd);
  point_fields_index_reader_->open();

  invert_fields_index_reader_ =
      new SerializedInvertFieldsIndexReader(files_.invert_index_file);
  invert_fields_index_reader_->open();

  field_values_index_reader_ =
      new SerializedFieldValuesIndexReadr(files_.fvm, files_.fvd);
  field_values_index_reader_->open();

  return 0;
}

int SegmentIndexReader::close() {
  if (point_fields_index_reader_) point_fields_index_reader_->close();
  if (invert_field_index_reader_) invert_field_index_reader_->close();
  if (field_values_index_reader_) field_values_index_reader_->close();
}

}  // namespace yas