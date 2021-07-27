#pragma once

#include "term_iterator.h"
#include <string>


namespace yas {
class Tokenizer {
 public:
  TermIterator* get_term_iterator(std::string text) = 0;
};
}  // namespace yas