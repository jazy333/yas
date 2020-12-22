#include "variable_byte_compression.h"

namespace yas {
int VariableByteCompression::compress(const std::string& in, std::string& out) {
  size_t outputSize = 0;
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  while (value > 127) {
    //|128: Set the next byte flag
    output[outputSize] = ((uint8_t)(value & 127)) | 128;
    // Remove the seveFn bits we just wrote
    value >>= 7;
    outputSize++;
  }
  output[outputSize++] = ((uint8_t)value) & 127;
  return outputSize;
}


int VariableByteCompression::decompress(const std::string& in,
                                        std::string& out) {
  int_t ret = 0;
  for (size_t i = 0; i < inputSize; i++) {
    ret |= (input[i] & 127) << (7 * i);
    // If the next-byte flag is set
    if (!(input[i] & 128)) {
      break;
    }
  }
  return ret;
}
}  // namespace yas