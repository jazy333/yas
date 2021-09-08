#include "delimeter_tokenizer.h"

#include "common/common.h"

namespace yas {
DelimeterTokenizer::DelimeterTokenizer() : delimeter_(" ") {}
DelimeterTokenizer::DelimeterTokenizer(const std::string& delimeter)
    : delimeter_(delimeter) {}
DelimeterTokenizer::~DelimeterTokenizer() {}
std::shared_ptr<TermIterator> DelimeterTokenizer::get_term_iterator(
    const std::string& text) {
  std::vector<Term> terms;
  std::vector<int> positions;
  std::vector<int> offsets;
  std::wstring wtext, wdelimeter;
  string2wstring(text, wtext);
  string2wstring(delimeter_, wdelimeter);
  int pos = 0;
  int i = 0;
  for (; i < wtext.size();) {
    Term t;
    std::wstring wterm_text;
    std::string term_text;

    int offset = 0;
    while (i < wtext.size()) {
      if (wtext[i] == wdelimeter[0]) {
        ++i;
        break;
      } else {
        wterm_text.push_back(wtext[i]);
      }
      ++i;
    }

    offset = i-1;
    
    if (wterm_text.size() == 0) {
      continue;
    }

    wstring2string(wterm_text, term_text);
    t.set_term(term_text);
    terms.push_back(t);
    positions.push_back(pos++);
    offsets.push_back(offset + 1);
  }

  return std::shared_ptr<SimpleTermIterator>(
      new SimpleTermIterator(terms, positions, offsets));
}
}  // namespace yas