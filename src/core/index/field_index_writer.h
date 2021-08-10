#pragma once
#include <cstdint>

#include "field.h"
#include "field_index_reader.h"
#include "field_info.h"
#include "file.h"
#include "index_writer_option.h"

namespace yas {
class FieldIndexWriter {
 public:
  virtual void flush(FieldInfo fi, uint32_t max_doc,Index,const IndexWriterOption& option) = 0;
  virtual void add(uint32_t docid, Field*) = 0;
};
}  // namespace yas