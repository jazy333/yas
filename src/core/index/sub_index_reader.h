#pragma once
#include "posting_list.h"
#include "term_reader.h"
#include "term.h"
#include "field_values_reader.h"

namespace yas {
class SubIndexReader {
 public:
  virtual TermReader* posting_list(Term*) = 0;
  virtual FieldIndexReadr* field_reader()=0;
};
}  // namespace yas