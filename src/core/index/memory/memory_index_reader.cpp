#include "memory_index_reader.h"

#include "memory_field_values_index_reader_wrapper.h"
#include "memory_point_values_index_reader_wrapper.h"

namespace yas {
MemoryIndexReader::MemoryIndexReader(
    std::shared_ptr<InvertFieldsIndexReader> invert_field_index_reader,
    std::unordered_map<std::string, PointFieldIndexReader*>*
        point_fields_index_readers,
    std::unordered_map<std::string, std::shared_ptr<FieldValueIndexReader>>*
        field_values_index_readers)
    : invert_field_index_reader_(invert_field_index_reader),
      point_fields_index_readers_(point_fields_index_readers),
      field_values_index_readers_(field_values_index_readers) {}

std::shared_ptr<InvertFieldsIndexReader>
MemoryIndexReader::invert_index_reader() {
  return invert_field_index_reader_;
}

std::shared_ptr<FieldValuesIndexReader>
MemoryIndexReader::field_values_reader() {
  return std::shared_ptr<MemoryFieldValuesIndexReaderWrapper>(
      new MemoryFieldValuesIndexReaderWrapper(field_values_index_readers_));
}

std::shared_ptr<PointFieldsIndexReader>
MemoryIndexReader::point_fields_reader() {
  return std::shared_ptr<MemoryPointValuesIndexReaderWrapper>(
      new MemoryPointValuesIndexReaderWrapper(point_fields_index_readers_));
}

int MemoryIndexReader::open() { return 0; }

int MemoryIndexReader::close() { return 0; }

SegmentInfo MemoryIndexReader::get_segment_info() { return info_; }

}  // namespace yas