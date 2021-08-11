#include "memory_index_reader.h"
#include "memory_field_values_index_reader_wrapper.h"
#include "memory_point_values_index_reader_wrapper.h"

namespace yas {
MemoryIndexReader::MemoryIndexReader(
    std::shared_ptr<InvertFieldsIndexWriter> invert_field_index_writer,
    std::unordered_map<std::string, FieldIndexWriter*>*
        point_fields_index_writers,
    std::unordered_map<std::string, FieldIndexWriter*>*
        field_values_index_writers)
    : invert_field_index_writer_(invert_field_index_writer),
      point_fields_index_writers_(point_fields_index_writers),
      field_values_index_writers_(field_values_index_writers) {}
MemoryIndexReader::~MemoryIndexReader() {}

std::shared_ptr<InvertFieldsIndexReader>
MemoryIndexReader::invert_index_reader(){
  return invert_field_index_writer_;
}

std::shared_ptr<FieldValuesIndexReader>
MemoryIndexReader::field_values_reader() {
  return std::shared_ptr<FieldValuesIndexReaderWrapper>(
      new FieldValuesIndexReaderWrapper(field_values_index_writers_));
}

std::shared_ptr<PointFieldsIndexReader>
MemoryIndexReader::point_fields_reader() {
  return std::shared_ptr<PointVlauesIndexReaderWrapper>(
      new PointVlauesIndexReaderWrapper(point_fields_index_writers_));
}

int MemoryIndexReader::open() { return 0; }

int MemoryIndexReader::close() { return 0; }

}  // namespace yas