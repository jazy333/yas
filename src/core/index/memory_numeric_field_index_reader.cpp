#include "memory_numeric_field_index_reader.h"

#include <algorithm>

namespace yas {
MemoryNumericFieldIndexReader::MemoryNumericFieldIndexReader(
    std::vector<uint32_t>* docids, std::vector<uint64_t>* values)
    : docids_(docids), values_(values) {}
    
uint64_t MemoryNumericFieldIndexReader::get(uint32_t docid) {
  auto iter = std::lower_bound(docids_->begin(), docids_.end(), docid);
  if (iter != docids_.end()) {
    int index = std::distance(iter - docids_->begin());
    return values_[index];
  }

  return -1;
}
}  // namespace yas