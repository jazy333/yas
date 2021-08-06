#include "memory_index_reader.h"

namespace yas {
MemoryIndexReader(
    std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers)
    : field_index_writer_(field_index_writer) {}
~MemoryIndexReader() {}

TermReader* posting_list(Term* term) {
  std::string field = term->get_field();
  if (field_index_writers_.count(field) == 1) {
    FieldIndexWriter* writer = field_index_writers_[field];
    return writer->get_reader(term);
  } else {
    return nullptr;
  }
}

FieldIndexReadr* field_reader(std::string field_name) {
  if (field_index_writers_.count(field_name) == 1) {
    return field_index_writers_[field_name];
  } else {
    return nullptr;
  }
}
}  // namespace yas