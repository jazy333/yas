#pragma once
#include <memory>
#include <string>

#include "term_iterator.h"

namespace yas {
class Tokenizer {
 public:
  virtual std::shared_ptr<TermIterator> get_term_iterator(
      const std::string& text) = 0;
};
}  // namespace yas