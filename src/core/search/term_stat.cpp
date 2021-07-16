#include "term_stat.h"

namespace yas {
TermStat::TermStat(/* args */) {}

TermStat::TermStat(Term term, int doc_freq, int total_term_freq)
    : term_(term), doc_freq_(doc_freq), total_term_freq_(total_term_freq) {}

Term TermStat::get_term() { return term_; }

int TermStat::get_doc_freq() { return doc_freq_; }

int TermStat::get_total_term_freq() { return total_term_freq_; }

}  // namespace yas