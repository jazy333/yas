#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

#include "core/index/field_index_reader.h"

namespace yas {
struct MatchedDoc {
  uint32_t docid_;
  float score_;
  std::unordered_map<std::string, int> doc_lens;
  std::unordered_map<std::string, int> match_term_counts;
  std::shared_ptr<FieldValuesIndexReader> field_value_reader;
};
}  // namespace yas
