#pragma once
#include <string>

#include "term_iterator.h"

namespace yas {
class Tokenizer {
 public:
  virtual TermIterator* get_term_iterator(const std::string& text) = 0;
};
}  // namespace yas