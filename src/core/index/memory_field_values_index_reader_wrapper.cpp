#include "memory_field_values_index_reader_wrapper.h"

namespace yas {
MemoryFieldValuesIndexReaderWrapper::FieldValueIndexReaderWrapper(
    std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers)
    : field_index_writers_(field_index_writers) {}
int MemoryFieldValuesIndexReaderWrapper::open() { return 0; }
int MemoryFieldValuesIndexReaderWrapper::close() { return 0; }

FieldValueIndexReader* MemoryFieldValuesIndexReaderWrapper::get_reader(
    const std::string& field_name) {
  if (field_index_writers_->count(field_name) == 1)
    return field_index_writers_[field_name];
  else
    return nullptr;
}
}  // namespace yas