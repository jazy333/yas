#pragma once
#include <string>
#include <cstdint>

namespace yas {
struct IndexStat {
  std::string field;
  long doc_count;
  uint32_t max_doc;
  long total_term_freq;
};

}  // namespace yas