#pragma once
#include <string>
#include <vector>

#include "term_iterator.h"
#include "tokenizer.h"

namespace yas {
class SimpleTokenizer : public Tokenizer {
 public:
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

  SimpleTokenizer();
  SimpleTokenizer(int interval);
  virtual ~SimpleTokenizer() = default;
  std::unique_ptr<TermIterator> get_term_iterator(
      const std::string& text) override;
  int get_interval();
  void set_interval(int interval);

 private:
  bool string2wstring(const std::string& s, std::wstring& result);
  bool wstring2string(const std::wstring& s, std::string& result);

 private:
  int interval_;
};
}  // namespace yas