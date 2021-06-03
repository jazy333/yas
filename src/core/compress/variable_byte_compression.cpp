#include "variable_byte_compression.h"

namespace yas {
void VariableByteCompression::compress(const uint32_t* in, size_t in_size,
                                       uint8_t* out, size_t& out_size) {
  uint32_t value = *in;
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  while (value > 127) {
    //|128: Set the next byte flag
    out[out_size] = ((uint8_t)(value & 127)) | 128;
    // Remove the seveFn bits we just wrote
    value >>= 7;
    out_size++;
  }
  out[out_size++] = ((uint8_t)value) & 127;
}

uint32_t* VariableByteCompression::decompress(const uint8_t* in, size_t in_size,
                                              uint32_t* out, size_t& out_size) {
  uint32_t ret = 0;
  for (size_t i = 0; i < in_size; i++) {
    ret |= (in[i] & 127) << (7 * i);
    // If the next-byte flag is set
    if (!(in[i] & 128)) {
      break;
    }
  }
  *out = ret;
  return nullptr;
}
}  // namespace yas