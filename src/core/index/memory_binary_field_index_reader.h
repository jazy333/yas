#pragma once
#include <vector>

#include "field_index_reader.h"

namespace yas {
class MemoryBinaryFieldIndexReader : public FieldIndexReader {
 public:
  MemoryBinaryFieldIndexReader(std::vector<uint32_t>* docids,
                                std::vector<uint64_t>* values));
  virtual ~MemoryBinaryFieldIndexReader();
  std::vector<uint8_t> get(uint32_t target);

 private:
 std::vector<uint32_t>* docids_;
  std::vector<std::vector<uint8_t>>* values_;
  
};
}  // namespace yas