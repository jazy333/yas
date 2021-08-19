#include "memory_point_values_index_reader_wrapper.h"

namespace yas {
MemoryPointValuesIndexReaderWrapper::MemoryPointValuesIndexReaderWrapper(
    std::unordered_map<std::string, PointFieldIndexReader*>*
        field_index_readers)
    : field_index_readers_(field_index_readers) {}

int MemoryPointValuesIndexReaderWrapper::open() { return 0; }

int MemoryPointValuesIndexReaderWrapper::close() { return 0; }

PointFieldIndexReader*
MemoryPointValuesIndexReaderWrapper::get_reader(
    const std::string& field_name,
    PointFieldIndexReader* init_reader) {
  if (field_index_readers_->count(field_name) == 1)
    return (*field_index_readers_)[field_name];
  else
    return nullptr;
}

}  // namespace yas