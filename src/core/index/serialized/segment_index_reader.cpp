#include "segment_index_reader.h"

#include "serialized_field_values_index_reader.h"
#include "serialized_invert_fields_index_reader.h"
#include "serialized_point_fields_index_reader.h"

namespace yas {
SegmentIndexReader::SegmentIndexReader(
    SegmentFiles files,
    const std::unordered_map<std::string, FieldInfo>& field_infos)
    : files_(files), field_infos_(field_infos) {
  for (auto&& item : field_infos) {
    id2name_[item.second.get_field_id()] = item.first;
  }
}

SegmentIndexReader::~SegmentIndexReader() {}

std::shared_ptr<InvertFieldsIndexReader>
SegmentIndexReader::invert_index_reader() {
  return invert_fields_index_reader_;
}

std::shared_ptr<FieldValuesIndexReader>
SegmentIndexReader::field_values_reader() {
  return field_values_index_reader_;
}

std::shared_ptr<PointFieldsIndexReader>
SegmentIndexReader::point_fields_reader() {
  return point_fields_index_reader_;
}

int SegmentIndexReader::open() {
  point_fields_index_reader_ = std::shared_ptr<PointFieldsIndexReader>(
      new SerializedPointFieldsIndexReader(field_infos_, files_.kdm, files_.kdi,
                                           files_.kdd));
  point_fields_index_reader_->open();

  invert_fields_index_reader_ = std::shared_ptr<InvertFieldsIndexReader>(
      new SerializedInvertFieldsIndexReader(files_.invert_index_file));
  invert_fields_index_reader_->open();

  field_values_index_reader_ = std::shared_ptr<FieldValuesIndexReader>(
      new SerializedFieldValuesIndexReader(files_.fvm, files_.fvd, id2name_));
  field_values_index_reader_->open();

  return 0;
}

int SegmentIndexReader::close() {
  if (point_fields_index_reader_) point_fields_index_reader_->close();
  if (invert_fields_index_reader_) invert_fields_index_reader_->close();
  if (field_values_index_reader_) field_values_index_reader_->close();
  return 0;
}

}  // namespace yas