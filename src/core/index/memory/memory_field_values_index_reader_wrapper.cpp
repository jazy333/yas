#include "memory_field_values_index_reader_wrapper.h"

namespace yas {
MemoryFieldValuesIndexReaderWrapper::MemoryFieldValuesIndexReaderWrapper(
    std::unordered_map<std::string, std::shared_ptr<FieldValueIndexReader>>*
        field_index_readers)
    : field_index_readers_(field_index_readers) {}

int MemoryFieldValuesIndexReaderWrapper::open() { return 0; }

int MemoryFieldValuesIndexReaderWrapper::close() { return 0; }

std::shared_ptr<FieldValueIndexReader>
MemoryFieldValuesIndexReaderWrapper::get_reader(const std::string& field_name) {
  if (field_index_readers_->count(field_name) == 1)
    return (*field_index_readers_)[field_name];
  else
    return nullptr;
}
}  // namespace yas