#pragma once
#include "compression.h"

namespace yas {
class EliasFanoCompression : public Compression {
 private:
  /* data */
  // unary code of high bits of nth docid within this byte
  static uint32_t decoding_table_high_bits[256][8];
  // docIdNumber = number of docid = number of 1 within one byte
  static uint8_t decoding_table_docid_number[256];
  // number of trailing zeros (zeros carryover), if whole byte=0 then
  // unaryCodeLength+=8
  static uint8_t decoding_table_high_bits_carry[256];
  static void elias_fano_create_decoding_table();

 public:
  EliasFanoCompression(/* args */);
  ~EliasFanoCompression();
  void compress(const uint32_t* in, size_t in_size, uint8_t* out,
                size_t& out_size) override;
  uint8_t* decompress(const uint8_t* in, size_t in_size, uint32_t* out,
                       size_t& out_size) override;
};

}  // namespace yas