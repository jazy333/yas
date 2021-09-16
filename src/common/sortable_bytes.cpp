#include "sortable_bytes.h"

namespace yas {
template <>
void sortable_bytes_encode(float d, u_char* out) {
  int bits;
  memcpy(&bits, &d, 4);
  int sortable_bits = (bits ^ (bits >> 31)) & 0x7fffffff;
  sortable_bytes_encode(sortable_bits, out);
}

template <>
void sortable_bytes_decode(const u_char* in, float& d) {
  int bits;
  sortable_bytes_decode(in, bits);
  int sortable_bits = (bits ^ (bits >> 31)) & 0x7fffffff;
  memcpy(&d, &sortable_bits, 4);
}

template <>
void sortable_bytes_encode(double d, u_char* out) {
  long bits;
  memcpy(&bits, &d, 8);
  long sortable_bits = (bits ^ (bits >> 63)) & 0x7fffffffffffffffL;
  sortable_bytes_encode(sortable_bits, out);
}

template <>
void sortable_bytes_decode(const u_char* in, double& d) {
  long bits;
  sortable_bytes_decode(in, bits);
  long sortable_bits = (bits ^ (bits >> 63)) & 0x7fffffffffffffffL;
  memcpy(&d, &sortable_bits, 8);
}
}  // namespace yas