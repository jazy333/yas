#include "fnv_hash.h"

namespace yas {
uint32_t FNVHash::hash32(const std::string& in, uint32_t seed) const{
  static const uint32_t kOffset = UINT32_C(2166136261);
  static const uint32_t kPrime = UINT32_C(16777619);

  const uint8_t* octets = reinterpret_cast<const uint8_t*>(in.data());
  int len = in.size();
  uint32_t hash = kOffset;

  for (int i = 0; i < len; ++i) {
    hash = hash ^ octets[i];
    hash = hash * kPrime;
  }

  return hash;
}

uint64_t FNVHash::hash64(const std::string& in, uint64_t seed) const{
  static const uint64_t kOffset = UINT64_C(14695981039346656037);
  static const uint64_t kPrime = UINT64_C(1099511628211);

  const uint8_t* octets = reinterpret_cast<const uint8_t*>(in.data());

  uint64_t hash = kOffset;
  int len = in.size();
  for (int i = 0; i < len; ++i) {
    hash = hash ^ octets[i];
    hash = hash * kPrime;
  }

  return hash;
}

__int128_t FNVHash::hash128(const std::string& in, __int128_t seed) const {
  return 0;
}




}  // namespace yas