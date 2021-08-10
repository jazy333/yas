#include "segment_index_reader.h"
#include "serialized_field_values_reader.h"
#include "serialized_invert_fields_index_reader.h"
#include "serialized_point_fields_index_reader.h"

namespace yas {
SegmentIndexReader::SegmentIndexReader(SegmentFiles files) : files_(files) {}

SegmentIndexReader::~SegmentIndexReader() {}

InvertFieldsIndexReader* SegmentIndexReader::invert_index_reader() {
  return invert_fields_index_reader_;
}

FieldValuesReader* SegmentIndexReader::field_values_reader() {
  return field_values_index_reader_;
}

PointFieldsIndexReader* SegmentIndexReader::point_fields_reader() {
  return point_fields_index_reader_;
}

int SegmentIndexReader::open() {
  point_fields_index_reader_ =
      new SerializedPointFieldsIndexReader(files_.kdm, files_.kdi, files_.kdd);
  point_fields_index_reader_->open();

  invert_fields_index_reader_ =
      new SerializedInvertFieldsIndexReader(files_.invert_index_file);
  invert_fields_index_reader_->open();

  field_values_index_reader_ =
      new SerializedFieldValuesIndexReadr(files_.fvm, files_, fvd);
  field_values_index_reader_->open();

  return 0;
}

int SegmentIndexReader::close() {
  if (point_fields_index_reader_) point_fields_index_reader_->close();
  if (invert_field_index_reader_) invert_field_index_reader_->close();
  if (field_values_index_reader_) field_values_index_reader_->close();
}

}  // namespace yas