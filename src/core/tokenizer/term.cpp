#include "term.h"

namespace yas {
Term::Term(std::string term, std::string field) : term_(term), field_(field) {}
Term::~Term() {}

std::string Term::get_term() { return term_; }
std::string Term::get_field() { return field_; }
void Term::set_term(const std::string& name) { name_ = name; }
void Term::set_field(const std::string& field) { filed_ = field; }
}  // namespace yas