#pragma once
#include <string>

#include "simple_term_iterator.h"
#include "tokenizer.h"

namespace yas {
class DelimeterTokenizer : public Tokenizer {
 public:
  DelimeterTokenizer();
  DelimeterTokenizer(const std::string& delimeter);
  virtual ~DelimeterTokenizer();
  std::shared_ptr<TermIterator> get_term_iterator(
      const std::string& text) override;

 private:
  std::string delimeter_;
};
}  // namespace yas