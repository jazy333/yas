#pragma once
#include <cstring>
#include <sys/types.h>
#include <type_traits>


namespace yas {
template <class T>
void sortable_bytes_encode(T d, u_char* out) {
  static_assert((std::is_same<T, long>::value || std::is_same<T, int>::value ||
                 std::is_same<T, short>::value),
                "sortable_bytes_encode only support:long,int,short");
  size_t size = sizeof(T);
  T one = 1;
  T flip = one << (size * 8 - 1);
  d ^= flip;
  for (int i = size - 1; i >= 0; --i) {
    out[size - i - 1] = static_cast<char>(d >> i * 8);
  }
}

template <class T>
void sortable_bytes_decode(const u_char* in, T& d) {
  static_assert((std::is_same<T, long>::value || std::is_same<T, int>::value ||
                 std::is_same<T, short>::value),
                "sortable_bytes_decode only support:long,int,short");
  d = T();
  size_t size = sizeof(T);
  T one = 1;
  T flip = one << (size * 8 - 1);
  for (size_t i = 1; i <= size; ++i) {
    d |= T((in[i - 1] & 0xff)) << ((size - i) * 8);
  }
  d ^= flip;
}

template <>
void sortable_bytes_encode(float d, u_char* out);

template <>
void sortable_bytes_decode(const u_char* in, float& d);

template <>
void sortable_bytes_encode(double d, u_char* out);

template <>
void sortable_bytes_decode(const u_char* in, double& d);
}  // namespace yas