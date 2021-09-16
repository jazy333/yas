#pragma once
#include <cstdint>
#include <string>

namespace yas {
struct IndexStat {
  std::string field;
  long doc_count = 0;
  uint32_t max_doc = 0;
  long total_term_freq = 0;
  int max_seg_no = 0;
};

}  // namespace yas