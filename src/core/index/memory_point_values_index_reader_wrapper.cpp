#include "memory_field_values_index_reader_wrapper.h"

namespace yas {
MemoryPointValuesIndexReaderWrapper::MemoryPointValuesIndexReaderWrapper(
    std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers)
    : field_index_writers_(field_index_writers) {}

int MemoryPointValuesIndexReaderWrapper::open() { return 0; }

int MemoryPointValuesIndexReaderWrapper::close() { return 0; }

PointValueIndexReader* MemoryPointValuesIndexReaderWrapper::get_reader(
    const std::string& field_name,
    std::unique_ptr<PointFieldIndexReader> init_reader) {
  if (field_index_writers_->count(field_name) == 1)
    return field_index_writers_[field_name];
  else
    return nullptr;
}

}  // namespace yas