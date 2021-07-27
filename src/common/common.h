#pragma once
#include <immintrin.h>
#include <sys/types.h>
#include <cstdint>

#define MM_LOAD_SI_128 _mm_loadu_si128
#define MM_STORE_SI_128 _mm_storeu_si128

namespace yas {
ssize_t pread_with_check(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite_with_check(int fd, const void *buf, size_t count, off_t offset);
ssize_t read_with_check(int fd, void *buf, size_t count);
ssize_t write_with_check(int fd, const void *buf, size_t count);
inline int round(int len, int align) { return ((len - 1) / align + 1) * align; }
template<class T> T gcd(T a, T b){
  if (b == 0)
    return a;
  else {
    return a > b ? gcd(b, a % b) : gcd(a, b % a);
  }
}
int lcm(int a, int b);
inline uint32_t asmbits(const uint32_t v);
inline uint32_t gccbits(const uint32_t v) {
  return v == 0 ? 0 : 32 - __builtin_clz(v);
}

inline uint32_t gccbits(const uint64_t v) {
  return v == 0 ? 0 : 64 - __builtin_clzl(v);
}
/**
 * Treats  __m128i as 4 x 32-bit integers and asks for the max
 * number of bits used (integer logarithm).
 */
inline uint32_t maxbit(const __m128i accumulator) {
  __attribute__((aligned(16))) uint32_t tmparray[4];
  MM_STORE_SI_128(reinterpret_cast<__m128i *>(tmparray), accumulator);
  return gccbits(tmparray[0] | tmparray[1] | tmparray[2] | tmparray[3]);
}

template <class T>
__attribute__((const)) inline bool need_padding_to_128Bits(const T *inbyte) {
  return (reinterpret_cast<uintptr_t>(inbyte) & 15) != 0;
}

uint8_t uint2uchar(uint32_t value);
uint32_t uchar2uint(uint8_t value);

}  // namespace yas