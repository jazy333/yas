#pragma once
#include "term.h"

namespace yas {
class TermStat {
 public:
  TermStat(Term term, int doc_freq, int total_term_freq);
  virtual ~TermStat() = default;
  Term get_term();
  int get_doc_freq();
  int get_total_term_freq();

 private:
  Term term_;
  int doc_freq_;
  int total_term_freq_;
};

}  // namespace yas