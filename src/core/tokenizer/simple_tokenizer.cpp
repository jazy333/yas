#include "simple_tokenizer.h"

#include <cstdlib>
#include <cstring>
#include <locale>
#include <string>

namespace yas {
SimpleTokenizer::SimpleTokenizer() : interval_(1) {}
SimpleTokenizer::SimpleTokenizer(int interval) : interval_(interval) {}
std::shared_ptr<TermIterator> SimpleTokenizer::get_term_iterator(
    const std::string& text) {
  std::vector<Term> terms;
  std::vector<int> positions;
  std::vector<int> offsets;
  std::wstring wtext;
  string2wstring(text, wtext);
  int pos = 0;
  int i = 0;
  for (; i < wtext.size();) {
    Term t;
    std::wstring wterm_text;
    std::string term_text;

    int inner = 0;
    int offset = 0;
    while (inner < interval_ && i < wtext.size()) {
      if (!std::iswblank(wtext[i]) && !std::iswpunct(wtext[i])) {
        if (iswalpha(wtext[i]))
          wterm_text.push_back(towlower(wtext[i]));
        else
          wterm_text.push_back(wtext[i]);
        ++inner;
        offset = i;
      }
      i++;
    }

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

bool SimpleTokenizer::string2wstring(const std::string& s,
                                     std::wstring& result) {
  setlocale(LC_ALL, "zh_CN.UTF-8");
  const char* source = s.c_str();
  size_t dst_size = s.size() + 1;
  wchar_t* dest = new wchar_t[dst_size];
  wmemset(dest, 0, dst_size);
  size_t ret = mbstowcs(dest, source, dst_size);
  if (ret == -1) {
    delete[] dest;
    return false;
  }
  result.assign(dest, ret);
  delete[] dest;

  return true;
}

bool SimpleTokenizer::wstring2string(const std::wstring& s,
                                     std::string& result) {
  std::string cur_locale = setlocale(LC_ALL, NULL);

  const wchar_t* source = s.c_str();
  size_t dst_size = sizeof(wchar_t) * s.size() + 1;
  char* dest = new char[dst_size];
  memset(dest, 0, dst_size);
  size_t ret = wcstombs(dest, source, dst_size);
  if (ret == -1) {
    delete[] dest;
    return false;
  }
  result.assign(dest, ret);
  delete[] dest;
  return true;
}

int SimpleTokenizer::get_interval() { return interval_; }
void SimpleTokenizer::set_interval(int interval) { interval_ = interval; }
}  // namespace yas