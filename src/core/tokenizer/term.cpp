#include "term.h"

namespace yas {
Term::Term(const std::string& text, const std::string& field)
    : term_(text), field_(field) {}

std::string Term::get_term() { return term_; }

std::string Term::get_field() { return field_; }

void Term::set_term(const std::string& name) { term_ = name; }

void Term::set_field(const std::string& field) { field_ = field; }
}  // namespace yas