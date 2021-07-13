#pragma once
#include "posting_list.h"
#include "term_reader.h"
#include "term.h"

namespace yas {
class SubIndexReader {
 public:
  virtual TermReader* posting_list(Term*) = 0;
};
}  // namespace yas