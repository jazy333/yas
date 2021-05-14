#pragma once

#include <sys/types.h>

#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <vector>

#include "sortable_bytes.h"

namespace yas {
template <class T, int D>
class Point {
 public:
  static const int dim;
  static const int bytes_per_dim;
  static const int bytes_length;

  Point() : docid_(-1) {}

  Point(std::initializer_list<T> v, int docid) {
    assert(v.size() == D);
    docid_=docid;
    std::copy(v.begin(), v.end(), std::begin(point.v));
  }

  Point(const Point& p) {
    docid_ = p.docid_;
    std::copy(std::begin(p.point.v), std::end(p.point.v), std::begin(point.v));
  }

  Point& operator=(const Point& p) {
    if (&p != this) {
      docid_ = p.docid_;
      std::copy(std::begin(p.point.v), std::end(p.point.v),
                std::begin(point.v));
    }
    return *this;
  }

  virtual ~Point() = default;

  int get_docid() { return docid_; }

  T& operator[](int index) {
    assert(index < D && index >= 0);
    return point.v[index];
  }

  Point operator-(const Point& p) {
    Point r;
    for (int i = 0; i < D; ++i) {
      r[i] = point.v[i] - p.point.v[i];
    }
    return r;
  }

  bool operator==(const Point& p) {
    return memcmp(p.point.bytes, point.bytes, bytes_size()) == 0;
  }

  bool operator!=(const Point& p) {
    return memcmp(p.point.bytes, point.bytes, bytes_size()) != 0;
  }

  u_char* bytes() { return point.bytes; }

  u_char get_byte(int dim, int pos) {
    int index = dim * bytes_per_dim + pos;
    return point.bytes[index];
  }

  /*
  *reverse get_byte
  */
  u_char get_byte_r(int dim, int pos) {
    int index = (dim+1) * bytes_per_dim + pos-1;
    return point.bytes[index];
  }

  u_char* get_bytes_one_dim(int dim) {
    assert(dim < D);
    return point.bytes + bytes_per_dim * dim;
  }

  size_t bytes_size() { return sizeof(T) * D; }

  int mismatch(const Point& p, int dim) {
    int index = (dim + 1) * bytes_per_dim - 1;
    int i = bytes_per_dim - 1;
    for (; i >= 0; --i) {
      index += i;
      if (point.bytes[index] != p.point.bytes[index]) break;
    }
    return bytes_per_dim - i;
  }

  void prefix_lens_per_dim(const Point& p, const std::vector<int>& ends,
                           std::vector<int>& prefix_lens) {
    assert(ends.size() == D && prefix_lens.size() == D);
    for (int i = 0; i < D; ++i) {
      int j = 0, end = ends[i];
      for (; j < end; ++j) {
        int index = (i + 1) * bytes_per_dim - j - 1;
        if (point.bytes[index] != p.point.bytes[index]) {
          break;
        }
      }
      prefix_lens[i] = j;
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const Point<T, D>& p) {
    os << '[';
    if (D == 1)
      std::copy(std::begin(p.point.v), std::end(p.point.v),
                std::ostream_iterator<T>(os, ""));
    else {
      std::copy(std::begin(p.point.v), std::end(p.point.v) - 1,
                std::ostream_iterator<T>(os, ","));
      os << p.point.v[D - 1];
    }
    os << "]#";
    os << p.docid_;
    return os;
  }

 private:
  int docid_;
  union {
    T v[D];
    u_char bytes[sizeof(T) * D];
  } point;
};

template <class T, int D>
const int Point<T, D>::dim = D;
template <class T, int D>
const int Point<T, D>::bytes_per_dim = sizeof(T);
template <class T, int D>
const int Point<T, D>::bytes_length = sizeof(T) * D;
}  // namespace yas