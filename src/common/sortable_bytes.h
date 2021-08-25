#pragma once
#include <cstring>
#include <sys/types.h>

namespace yas {
template <class T>
void sortable_bytes_encode(T d, u_char* out);

template <class T>
void sortable_bytes_decode(const u_char* in, T& d);

template <>
void sortable_bytes_encode(float d, u_char* out);

template <>
void sortable_bytes_decode(const u_char* in, float& d);

template <>
void sortable_bytes_encode(double d, u_char* out);

template <>
void sortable_bytes_decode(const u_char* in, double& d);
}  // namespace yas