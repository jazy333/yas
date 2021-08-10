#include "field_values_index_reader_wrapper.h"

namespace yas {
FieldValuesIndexReaderWrapper::FieldValueIndexReaderWrapper(
    std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers)
    : field_index_writers_(field_index_writers) {}
int FieldValuesIndexReaderWrapper::open() { return 0; }
int FieldValuesIndexReaderWrapper::close() { return 0; }

FieldValueIndexReader* FieldValuesIndexReaderWrapper::get_reader(
    const std::string& field_name) {
  if (field_index_writers_->count(field_name) == 1)
    return field_index_writers_[field_name];
  else
    return nullptr;
}
}  // namespace yas