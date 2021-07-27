#include "document.h"

namespace yas {
Document::Document(/* args */) {}

Document::~Document() {}

void Document::add_field(Field* field) { fields_.push_back(field); }
std::vector<Field*> Document::get_fields() { return fields_; }
}  // namespace yas