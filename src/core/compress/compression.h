#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace yas {
class Compression {
 public:
  virtual void compress(uint32_t* in, size_t in_size, uint8_t* out,
                        size_t& out_size) = 0;
  virtual uint8_t* decompress(const uint8_t* in, size_t in_size, uint32_t* out,
                              size_t& out_size) = 0;
  void compress(const std::vector<uint32_t>& in, std::vector<uint8_t>& out) {
    size_t available = 2 * in.size();
    out.resize(available);
    compress(const_cast<uint32_t*>(in.data()), in.size(), out.data(),
             available);
    out.shrink_to_fit();
  }
  void decompress(const std::vector<uint8_t>& in, std::vector<uint32_t>& out) {
    size_t available = 4 * in.size();
    out.resize(available);
    decompress(in.data(), in.size(), out.data(), available);
    out.shrink_to_fit();
  }
};
}  // namespace yas