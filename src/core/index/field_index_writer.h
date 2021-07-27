#pragma once
#include "field.h"
#include "field_index_reader.h"

namespace yas {
class FieldIndexWriter {
 public:
  virtual void flush() = 0;
  virtual void add(uint32_t docid,Field*) = 0;
  virtual FieldIndexReader* get_reader() = 0;
};
}  // namespace yas