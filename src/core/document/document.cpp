#include "document.h"

namespace yas {
Document::Document(/* args */) {}

Document::~Document() { fields_.clear(); }

void Document::add_field(std::shared_ptr<Field> field) {
  fields_.push_back(field);
}

std::vector<std::shared_ptr<Field>> Document::get_fields() { return fields_; }
}  // namespace yas