#pragma once
#include <cstdint>

#include "hash.h"
//fnv1a 
namespace yas {
class FNVHash : public Hash {
 public:
  virtual uint32_t hash32(const std::string& in, uint32_t seed = 0) const;
  virtual uint64_t hash64(const std::string& in, uint64_t seed = 0) const;
  virtual __int128_t hash128(const std::string& in, __int128_t seed = 0) const;
};
}  // namespace yas