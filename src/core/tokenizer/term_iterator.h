#pragma once
#include "term.h"

namespace yas {
class TermIterator {
 public:
  bool next() = 0;
  int position() = 0;
  int offset() = 0;
  Term term()=0;
};
}  // namespace yas