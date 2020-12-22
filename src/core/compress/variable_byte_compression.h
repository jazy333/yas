#pragma once
#include "compression.h"

namespace yas {
class VariableByteCompression : public Compression {
 public:
  virtual int compress(const std::string& in, std::string& out);
  virtual int decompress(const std::string& in, std::string& out);
};
}  // namespace yas