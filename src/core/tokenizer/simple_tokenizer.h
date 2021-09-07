#pragma once
#include <string>
#include <vector>

#include "term_iterator.h"
#include "tokenizer.h"
#include "simple_term_iterator.h"

namespace yas {
class SimpleTokenizer : public Tokenizer {
 public:
  SimpleTokenizer();
  SimpleTokenizer(int interval);
  virtual ~SimpleTokenizer() = default;
  std::shared_ptr<TermIterator> get_term_iterator(
      const std::string& text) override;
  int get_interval();
  void set_interval(int interval);

 private:
  int interval_;
};
}  // namespace yas