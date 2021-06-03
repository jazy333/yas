#pragma once
#include "compression.h"

namespace yas {
class VariableByteCompression : public Compression {
 public:
  void compress(const uint32_t* in, size_t in_size, uint8_t* out,
                size_t& out_size) override;
  uint32_t* decompress(const uint8_t* in, size_t in_size, uint32_t* out,
                       size_t& out_size) override;
};
}  // namespace yas