#include "elias_fano_compression.h"

#include <cmath>
#include <mutex>
// EliasFanoCompression: quasi-succinct compression of sorted integers in CPP
//
// Elias-Fano encoding is quasi succinct, which means it is almost as good as
// the best theoretical possible compression scheme for sorted integers. While
// it can be used to compress any sorted list of integers, we will use it for
// compressing posting lists of inverted indexes. Based on a research paper by
// Sebastiano Vigna:
// http://vigna.di.unimi.it/ftp/papers/QuasiSuccinctIndices.pdf
//

namespace yas {
EliasFanoCompression::EliasFanoCompression(/* args */) {
  std::once_flag f;
  std::call_once(f, elias_fano_create_decoding_table);
}

EliasFanoCompression::~EliasFanoCompression() {}
void EliasFanoCompression::compress(const uint32_t* in, size_t in_size,
                                    uint8_t* out, size_t& out_size) {
  // Elias Fano Coding
  // compress sorted integers: Given n and u we have a monotone sequence 0 ≤
  // x0, x1, x2, ... , xn-1 ≤ u at most 2 + log(u / n) bits per element
  // Quasi-succinct: less than half a bit away from succinct bound!
  // https://en.wikipedia.org/wiki/Unary_coding
  // http://vigna.di.unimi.it/ftp/papers/QuasiSuccinctIndices.pdf
  // http://shonan.nii.ac.jp/seminar/029/wp-content/uploads/sites/12/2013/07/Sebastiano_Shonan.pdf
  // http://www.di.unipi.it/~ottavian/files/elias_fano_sigir14.pdf
  // http://hpc.isti.cnr.it/hpcworkshop2014/PartitionedEliasFanoIndexes.pdf

  uint32_t last_docid = 0;

  uint64_t buffer1 = 0;
  int buffer_length1 = 0;
  uint64_t buffer2 = 0;
  int buffer_length2 = 0;

  uint32_t largest_docid = in[in_size - 1];
  double average_delta = (double)largest_docid / (double)in_size;
  double average_delta_log = log2(average_delta);
  int low_bits_length = (int)floor(average_delta_log);
  if (low_bits_length < 0) low_bits_length = 0;
  uint64_t low_bits_mask = (((uint64_t)1 << low_bits_length) - 1);

  int compressed_buffer_pointer1 = 0;

  // +6 : for docid number, lowerBitsLength and ceiling
  int compressed_buffer_pointer2 = low_bits_length * in_size / 8 + 6;

  // store postingList.Count for decompression: LSB first
  out[compressed_buffer_pointer1++] = (uint8_t)(in_size & 0xff);
  out[compressed_buffer_pointer1++] = (uint8_t)((in_size >> 8) & 0xff);
  out[compressed_buffer_pointer1++] = (uint8_t)((in_size >> 16) & 0xff);
  out[compressed_buffer_pointer1++] = (uint8_t)((in_size >> 24) & 0xff);

  // store low_bits_length for decompression
  out[compressed_buffer_pointer1++] = (uint8_t)low_bits_length;

  for (size_t i = 0; i < in_size; ++i) {
    // docID strictly monotone/increasing numbers, docIdDelta strictly
    // positive, no zero allowed
    uint32_t docid = in[i];
    uint32_t docid_delta = (docid - last_docid - 1);

    // low bits
    // Store the lower l= log(u / n) bits explicitly
    // binary packing/bit packing

    buffer1 <<= low_bits_length;
    buffer1 |= (docid_delta & low_bits_mask);
    buffer_length1 += low_bits_length;

    // flush buffer to out
    while (buffer_length1 > 7) {
      buffer_length1 -= 8;
      out[compressed_buffer_pointer1++] = (uint8_t)(buffer1 >> buffer_length1);
    }

    // high bits
    // Store high bits as a sequence of unary coded gaps
    // 0=1, 1=01, 2=001, 3=0001, ...
    // https://en.wikipedia.org/wiki/Unary_coding

    // length of unary code
    uint32_t unary_code_length = (uint32_t)(docid_delta >> low_bits_length) + 1;
    buffer2 <<= (int)unary_code_length;

    // set most right bit
    buffer2 |= 1;
    buffer_length2 += (int)unary_code_length;

    // flush buffer to out
    while (buffer_length2 > 7) {
      buffer_length2 -= 8;
      out[compressed_buffer_pointer2++] = (uint8_t)(buffer2 >> buffer_length2);
    }

    last_docid = docid;
  }

  // final flush buffer
  if (buffer_length1 > 0) {
    out[compressed_buffer_pointer1++] =
        (uint8_t)(buffer1 << (8 - buffer_length1));
  }

  if (buffer_length2 > 0) {
    out[compressed_buffer_pointer2++] =
        (uint8_t)(buffer2 << (8 - buffer_length2));
  }
}
uint32_t* EliasFanoCompression::decompress(const uint8_t* in, size_t in_size,
                                           uint32_t* out, size_t& out_size) {
  // array is faster than list,
  // but wastes space with fixed size
  // this is only important for decompression, not for compressed
  // intersection (because we have only a fraction of results)

  int low_bits_pointer = 0;
  uint32_t last_docid = 0;
  uint32_t docid = 0;
  size_t result_size = 0;

  // read postingList.Count for decompression: LSB first
  int posting_list_count = in[low_bits_pointer++];
  posting_list_count |= (int)in[low_bits_pointer++] << 8;
  posting_list_count |= (int)in[low_bits_pointer++] << 16;
  posting_list_count |= (int)in[low_bits_pointer++] << 24;

  // read fanoParamInt for decompression
  uint8_t low_bits_length = in[low_bits_pointer++];

  // decompress low bits
  uint8_t low_bits_count = 0;
  uint8_t low_bits = 0;

  // decompress high bits
  uint8_t cb = 1;
  for (int high_bits_pointer = low_bits_length * posting_list_count / 8 + 6;
       high_bits_pointer < in_size; high_bits_pointer++) {
    // number of trailing zeros (zeros carryover), if whole byte=0 then
    // unaryCodeLength+=8
    docid += decoding_table_high_bits_carry[cb];
    cb = in[high_bits_pointer];

    // number of docids contained within one byte
    uint8_t docid_number = decoding_table_docid_number[cb];
    for (uint8_t i = 0; i < docid_number; i++) {
      // decompress low bits
      docid <<= low_bits_count;
      docid |=
          low_bits &
          ((1u << low_bits_count) -
           1u);  // mask remainder from previous lowBits, then add/or to docid

      while (low_bits_count < low_bits_length) {
        docid <<= 8;
        low_bits = in[low_bits_pointer++];
        docid |= low_bits;
        low_bits_count += 8;
      }
      low_bits_count -= low_bits_length;
      // move right bits which belong to next docid
      docid >>= low_bits_count;

      // decompress high bits
      // 1 byte contains high bits in unary code of 0..8 docid's
      docid += (decoding_table_high_bits[cb][i] << low_bits_length) +
               last_docid + 1u;
      out[result_size++] = (uint32_t)docid;
      last_docid = docid;
      docid = 0;
    }
  }
  out_size = result_size;
  return out + out_size;
}

void EliasFanoCompression::elias_fano_create_decoding_table() {
  for (int i = 0; i < 256; i++) {
    uint8_t zero_count = 0;
    for (int j = 7; j >= 0; j--) {
      // count 1 within i
      if ((i & (1 << j)) > 0) {
        // unary code of high bits of nth docid within this byte
        decoding_table_high_bits[i][decoding_table_docid_number[i]] =
            zero_count;
        // docIdNumber = number of docid = number of 1 within one byte
        decoding_table_docid_number[i]++;
        zero_count = 0;
      } else {
        // count 0 since last 1 within i
        zero_count++;
      }
    }
    // number of trailing zeros (zeros carryover), if whole byte=0 then
    // unaryCodeLength+=8
    decoding_table_high_bits_carry[i] = zero_count;
  }
}

}  // namespace yas
