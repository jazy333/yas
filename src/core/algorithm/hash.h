#pragma once
#include <string>

namespace yas {
class Hash {
 public:
  virtual uint32_t hash32(const std::string& in, uint32_t seed) = 0;
  virtual uint64_t hash64(const std::string& in, uint64_t seed) = 0;
  virtual __int128_t hash128(const std::string& in, __int128_t seed) = 0;
};
}  // namespace yas