#include "segment_index_reader.h"

#include "mmap_file.h"
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

int SegmentIndexReader::read_segment_info() {
  auto segment_file_handle = std::unique_ptr<File>(new MMapFile);
  int ret = segment_file_handle->open(files_.si, false);
  if (ret < 0) return ret;
  segment_file_handle->read(&info_.max_docid, sizeof(info_.max_docid));
  segment_file_handle->read(&info_.last_update_time,
                            sizeof(info_.last_update_time));
  segment_file_handle->close();
  return 0;
}

int SegmentIndexReader::open() {
  point_fields_index_reader_ = std::shared_ptr<PointFieldsIndexReader>(
      new SerializedPointFieldsIndexReader(field_infos_, files_.kdm, files_.kdi,
                                           files_.kdd));
  int ret = point_fields_index_reader_->open();
  if (ret < 0) {
    point_fields_index_reader_ = nullptr;
  }

  invert_fields_index_reader_ = std::shared_ptr<InvertFieldsIndexReader>(
      new SerializedInvertFieldsIndexReader(files_.invert_index_file));
  ret = invert_fields_index_reader_->open();
  if (ret < 0) {
    invert_fields_index_reader_ = nullptr;
  }

  field_values_index_reader_ = std::shared_ptr<FieldValuesIndexReader>(
      new SerializedFieldValuesIndexReader(files_.fvm, files_.fvd, id2name_));
  ret = field_values_index_reader_->open();
  if (ret < 0) {
    field_values_index_reader_ = nullptr;
  }
  ret = read_segment_info();

  return ret;
}

int SegmentIndexReader::close() {
  if (point_fields_index_reader_) point_fields_index_reader_->close();
  if (invert_fields_index_reader_) invert_fields_index_reader_->close();
  if (field_values_index_reader_) field_values_index_reader_->close();
  return 0;
}

SegmentInfo SegmentIndexReader::get_segment_info() { return info_; }

}  // namespace yas