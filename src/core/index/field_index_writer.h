#pragma once
#include <cstdint>
#include <memory>

#include "core/document/field.h"
#include "core/index/field_index_reader.h"
#include "core/document/field_info.h"
#include "core/db/file.h"
#include "index_option.h"

namespace yas {
class FieldIndexWriter {
 public:
  virtual ~FieldIndexWriter() = default;
  virtual void flush(FieldInfo fi, uint32_t max_doc,
                     const IndexOption& option) = 0;
  virtual int add(uint32_t docid, std::shared_ptr<Field> field) = 0;
};
}  // namespace yas