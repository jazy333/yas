#pragma once
#include <cstdint>
#include <vector>

namespace yas {
class MutableLongValuesCompression {
 public:
  MutableLongValuesCompression();
  virtual ~MutableLongValuesCompression();
  void add(uint64_t value);
  void compress(uint8_t* out, size_t& out_size);
  void decompress(const uint8_t* in, size_t in_size);

 private:
  int max_bits_;
  std::vector<uint64_t> values_;
  uint64_t max_;
  uint64_t min_;
  uint64_t gcd_;
};
}  // namespace yas