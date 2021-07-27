#include "text_field.h"
#include "invert_field_index_writer.h"

namespace yas {
TextField::TextField(std::string name, std::string value)
    : Field(name), value_(value) {}

FieldIndexWriter* TextField::get_field_index_writer() {
  return new InvertFieldIndexWriter();
}
}  // namespace yas