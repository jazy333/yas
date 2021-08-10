#include "field_values_index_reader_wrapper.h"

namespace yas {
PointValuesIndexReaderWrapper::PointValuesIndexReaderWrapper(
    std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers)
    : field_index_writers_(field_index_writers) {}
int PointValuesIndexReaderWrapper::open() { return 0; }
int PointValuesIndexReaderWrapper::close() { return 0; }

PointValueIndexReader* PointValuesIndexReaderWrapper::get_reader(
    const std::string& field_name) {
  if (field_index_writers_->count(field_name) == 1)
    return field_index_writers_[field_name];
  else
    return nullptr;
}
}  // namespace yas