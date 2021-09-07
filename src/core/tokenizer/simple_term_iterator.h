#pragma once
#include <string>
#include <vector>

#include "term.h"
#include "term_iterator.h"

namespace yas {
class SimpleTermIterator : public TermIterator {
 public:
  SimpleTermIterator() : index_(-1) {}
  SimpleTermIterator(std::vector<Term>& terms, std::vector<int>& positions,
                     std::vector<int>& offsets)
      : index_(-1), terms_(terms), positions_(positions), offsets_(offsets) {}
  ~SimpleTermIterator() = default;
  bool next() {
    ++index_;
    return index_ < terms_.size();
  }
  int position() { return positions_[index_]; }
  int offset() { return offsets_[index_]; }
  Term term() { return terms_[index_]; }

 private:
  int index_;
  std::vector<Term> terms_;
  std::vector<int> positions_;
  std::vector<int> offsets_;
};
}  // namespace yas