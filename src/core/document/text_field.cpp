#include "text_field.h"

namespace yas {

TextField::TextField(const std::string& name, const std::string& value)
    : Field(name), value_(value) {}

std::string TextField::get_value() { return value_; }

FieldIndexWriter* TextField::make_field_index_writer() {
  return nullptr;
}

}  // namespace yas