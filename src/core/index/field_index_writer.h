#pragma once
#include <cstdint>

#include "field.h"
#include "field_index_reader.h"
#include "field_info.h"
#include "file.h"

namespace yas {
class FieldIndexWriter {
 public:
  virtual void flush(File* fvm, File* fvd, FieldInfo fi, uint32_t max_doc) = 0;
  virtual void add(uint32_t docid, Field*) = 0;
  virtual FieldIndexReader* get_reader() = 0;
};
}  // namespace yas