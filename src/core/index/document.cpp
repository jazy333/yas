#include "document.h"

namespace yas {
Document::Document(/* args */) {}

Document::~Document() { fields_.clear(); }

void Document::add_field(std::unique_ptr<Field> field) {
  fields_.push_back(std::move(field));
}

std::vector<std::unique_ptr<Field>> Document::get_fields() { return fields_; }
}  // namespace yas