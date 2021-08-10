#pragma once
#include <string>

#include "field_index_reader.h"
#include "posting_list.h"
#include "term.h"
#include "term_reader.h"

namespace yas {
class SubIndexReader {
 public:
  virtual InvertFieldsIndexReader* invert_index_reader() = 0;
  virtual FieldValuesIndexReader* field_values_reader() = 0;
  virtual PointFieldsIndexReader* point_fields_reader() = 0;
  virtual int open() = 0;
  virtual int close() = 0;
};
}  // namespace yas