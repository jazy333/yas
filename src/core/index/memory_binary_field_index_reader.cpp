#include "memory_binary_field_index_reader.h"

namespace yas {
MemoryBinaryFieldIndexReader::MemoryBinaryFieldIndexReader(
    std::vector<uint32_t>* docids, std::vector<uint64_t>* values)
    : docids_(docids), values_(values) {}
MemoryBinaryFieldIndexReader::~MemoryBinaryFieldIndexReader() {}
std::vector<uint8_t> MemoryBinaryFieldIndexReader::get(uint32_t target) {
  auto iter = std::lower_bound(docids_->begin(), docids_.end(), docid);
  if (iter != docids_.end()) {
    int index = std::distance(iter - docids_->begin());
    return values_[index];
  }

  return {};
}
}  // namespace yas