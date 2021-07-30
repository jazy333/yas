#pragma once

#include "field_index_reader.h"
#include <vector>

namespace yas {
class MemoryNumericFieldIndexReader : public FieldIndexReader {
 public:
  MemoryNumericFieldIndexReader(std::vector<uint32_t>* docids,
                                std::vector<uint64_t>* values);
  uint64_t get(uint32_t docid);

 private:
  std::vector<uint32_t>* docids_;
  std::vector<uint64_t>* values_;
}
}  // namespace yas
