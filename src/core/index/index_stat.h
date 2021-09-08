#pragma once
#include <cstdint>
#include <string>

namespace yas {
struct IndexStat {
  std::string field;
  long doc_count;
  uint32_t max_doc;
  long total_term_freq;
  int max_seg_no;
};

}  // namespace yas