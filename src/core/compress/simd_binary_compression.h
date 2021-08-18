#pragma once
#include <emmintrin.h>  // SSE2
#include <immintrin.h>
#include <xmmintrin.h>  // SSE

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "common.h"
#include "compression.h"
#include "simd_bitpacking.h"
#define SIMD_BLOCKSIZE 128
namespace yas {

// Folklore code, unknown origin of this idea
__attribute__((always_inline)) static inline __m128i simd_prefix_sum(
    __m128i curr, __m128i prev) {
  const __m128i _tmp1 = _mm_add_epi32(_mm_slli_si128(curr, 8), curr);
  const __m128i _tmp2 = _mm_add_epi32(_mm_slli_si128(_tmp1, 4), _tmp1);
  return _mm_add_epi32(_tmp2, _mm_shuffle_epi32(prev, 0xff));
}

__attribute__((always_inline)) static inline __m128i simd_delta(__m128i curr,
                                                                __m128i prev) {
  return _mm_sub_epi32(
      curr, _mm_or_si128(_mm_slli_si128(curr, 4), _mm_srli_si128(prev, 12)));
}

static __m128i run_prefix_sum(__m128i initOffset, uint32_t* pData,
                              size_t TotalQty) {
  const size_t QtyDivBy4 = TotalQty / 4;
  // The block should contain 8N 32-bit integers, where N is some integer
  assert(QtyDivBy4 % 2 == 0);

  __m128i* pCurr = reinterpret_cast<__m128i*>(pData);
  const __m128i* pEnd = pCurr + QtyDivBy4;

  // Leonid Boytsov: manual loop unrolling may be crucial here.
  while (pCurr < pEnd) {
    initOffset = simd_prefix_sum(MM_LOAD_SI_128(pCurr), initOffset);
    MM_STORE_SI_128(pCurr++, initOffset);

    initOffset = simd_prefix_sum(MM_LOAD_SI_128(pCurr), initOffset);
    MM_STORE_SI_128(pCurr++, initOffset);
  }

  return initOffset;
}

template <bool delta>
class SIMDBinaryCompression : public Compression {
 public:
  SIMDBinaryCompression() : init_(0) {}
  SIMDBinaryCompression(uint32_t init) : init_(init) {}
  virtual void compress(uint32_t* in, size_t in_size, uint8_t* out,
                        size_t& out_size) override {
    uint32_t* iout = reinterpret_cast<uint32_t*>(out);
    uint32_t* out_start = iout;
    *iout++ = in_size;
    __m128i init = _mm_setr_epi32(init_, init_, init_, init_);
    for (size_t i = 0; i < in_size; i += SIMD_BLOCKSIZE) {
      uint32_t bits = maxbits(in + i, init);
      *iout++ = bits;
      simdpackwithoutmask(in + i, reinterpret_cast<__m128i*>(iout), bits);
      MM_LOAD_SI_128(reinterpret_cast<__m128i*>(const_cast<uint32_t*>(in + i)));
      iout += (SIMD_BLOCKSIZE * bits / 32);
    }
    out_size = (iout - out_start) * 4;
  }

  virtual uint8_t* decompress(const uint8_t* in, size_t in_size, uint32_t* out,
                              size_t& out_size) override {
    const uint32_t* iin = reinterpret_cast<const uint32_t*>(in);
    uint32_t* out_start = out;
    size_t length = *iin++;
    __m128i init = _mm_setr_epi32(init_, init_, init_, init_);
    for (size_t i = 0; i < length; i += SIMD_BLOCKSIZE) {
      uint32_t bit = *iin++;
      simdunpack(reinterpret_cast<const __m128i*>(iin), out, bit);
      if (delta) {
        init = run_prefix_sum(init, out, SIMD_BLOCKSIZE);
      }
      iin += (SIMD_BLOCKSIZE * bit / 32);
      out += SIMD_BLOCKSIZE;
    }
    out_size = out - out_start;
    return reinterpret_cast<uint8_t*>(const_cast<uint32_t*>(iin));
  }

  uint32_t get_init() { return init_; }
  void set_init(uint32_t init) { init_ = init; }

 private:
  uint32_t maxbits(uint32_t* in, __m128i& initoffset) {
    __m128i* pin = reinterpret_cast<__m128i*>(in);
    __m128i newvec = MM_LOAD_SI_128(pin);
    __m128i accumulator = newvec;
    if (delta) {
      accumulator = simd_delta(newvec, initoffset);
      MM_STORE_SI_128(pin, accumulator);
    }
    __m128i oldvec = newvec;
    for (uint32_t k = 1; 4 * k < SIMD_BLOCKSIZE; ++k) {
      newvec = MM_LOAD_SI_128(pin + k);
      __m128i diff = newvec;
      if (delta) {
        diff = simd_delta(newvec, oldvec);
        MM_STORE_SI_128(pin + k, diff);
      }
      accumulator = _mm_or_si128(accumulator, diff);
      oldvec = newvec;
    }
    initoffset = oldvec;
    return maxbit(accumulator);
  }
  uint32_t init_;
};
}  // namespace yas
