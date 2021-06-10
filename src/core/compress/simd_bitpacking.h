#pragma once
#include <fcntl.h>
#include <immintrin.h>

#include <cerrno>
#include <cstdint>

#define MM_LOAD_SI_128 _mm_loadu_si128
#define MM_STORE_SI_128 _mm_storeu_si128

/**
 * This code is borrowed from  SIMDCompressionAndIntersection
 * See more details :https://github.com/lemire/SIMDCompressionAndIntersection
 *(c) Daniel Lemire
 */
namespace yas {
void __SIMD_fastpackwithoutmask0(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask1(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask2(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask3(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask4(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask5(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask6(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask7(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask8(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask9(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask10(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask11(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask12(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask13(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask14(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask15(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask16(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask17(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask18(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask19(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask20(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask21(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask22(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask23(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask24(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask25(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask26(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask27(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask28(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask29(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask30(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask31(const uint32_t *, __m128i *);
void __SIMD_fastpackwithoutmask32(const uint32_t *, __m128i *);

void __SIMD_fastunpack1(const __m128i *, uint32_t *);
void __SIMD_fastunpack2(const __m128i *, uint32_t *);
void __SIMD_fastunpack3(const __m128i *, uint32_t *);
void __SIMD_fastunpack4(const __m128i *, uint32_t *);
void __SIMD_fastunpack5(const __m128i *, uint32_t *);
void __SIMD_fastunpack6(const __m128i *, uint32_t *);
void __SIMD_fastunpack7(const __m128i *, uint32_t *);
void __SIMD_fastunpack8(const __m128i *, uint32_t *);
void __SIMD_fastunpack9(const __m128i *, uint32_t *);
void __SIMD_fastunpack10(const __m128i *, uint32_t *);
void __SIMD_fastunpack11(const __m128i *, uint32_t *);
void __SIMD_fastunpack12(const __m128i *, uint32_t *);
void __SIMD_fastunpack13(const __m128i *, uint32_t *);
void __SIMD_fastunpack14(const __m128i *, uint32_t *);
void __SIMD_fastunpack15(const __m128i *, uint32_t *);
void __SIMD_fastunpack16(const __m128i *, uint32_t *);
void __SIMD_fastunpack17(const __m128i *, uint32_t *);
void __SIMD_fastunpack18(const __m128i *, uint32_t *);
void __SIMD_fastunpack19(const __m128i *, uint32_t *);
void __SIMD_fastunpack20(const __m128i *, uint32_t *);
void __SIMD_fastunpack21(const __m128i *, uint32_t *);
void __SIMD_fastunpack22(const __m128i *, uint32_t *);
void __SIMD_fastunpack23(const __m128i *, uint32_t *);
void __SIMD_fastunpack24(const __m128i *, uint32_t *);
void __SIMD_fastunpack25(const __m128i *, uint32_t *);
void __SIMD_fastunpack26(const __m128i *, uint32_t *);
void __SIMD_fastunpack27(const __m128i *, uint32_t *);
void __SIMD_fastunpack28(const __m128i *, uint32_t *);
void __SIMD_fastunpack29(const __m128i *, uint32_t *);
void __SIMD_fastunpack30(const __m128i *, uint32_t *);
void __SIMD_fastunpack31(const __m128i *, uint32_t *);
void __SIMD_fastunpack32(const __m128i *, uint32_t *);
void simdpackwithoutmask(const uint32_t *__restrict__ in,
                         __m128i *__restrict__ out, const uint32_t bit);
void simdunpack(const __m128i *__restrict__ in, uint32_t *__restrict__ out,
                const uint32_t bit);
}  // namespace yas