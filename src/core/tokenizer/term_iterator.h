#pragma once
#include "term.h"

namespace yas {
class TermIterator {
 public:
  virtual bool next() = 0;
  virtual int position() = 0;
  virtual int offset() = 0;
  virtual Term term() = 0;
};
}  // namespace yas