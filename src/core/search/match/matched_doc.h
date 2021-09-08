#pragma once
#include <cstdint>
#include "core/index/field_index_reader.h"

namespace yas {
struct MatchedDoc {
  uint32_t docid_;
  float score_;
  std::shared_ptr<FieldValuesIndexReader> field_value_reader;
};
}  // namespace yas
