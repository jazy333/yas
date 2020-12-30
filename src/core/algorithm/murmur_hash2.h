#pragma once
#include "hash.h"

namespace yas {
class MurmurHash2 : public Hash {
 public:
  virtual uint32_t hash32(const std::string& in, uint32_t seed) const;
  virtual uint64_t hash64(const std::string& in, uint64_t seed) const;
  virtual __int128_t hash128(const std::string& in, __int128_t seed) const;
};
}  // namespace yas