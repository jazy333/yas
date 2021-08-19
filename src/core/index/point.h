#pragma once

#include <sys/types.h>

#include <cassert>
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

  Point() : docid_(-1) {
    for (int i = 0; i < D; ++i) {
      u_char* dim_data = point.bytes + i * bytes_per_dim;
      sortable_bytes_encode(T(), dim_data);
    }
  }

  Point(u_char* sortable_bytes) : docid_(-1) {
    memcpy(point.bytes, sortable_bytes, bytes_length);
  }

  Point(std::initializer_list<T> v, int docid) {
    assert(v.size() == D);
    docid_ = docid;
    // std::copy(v.begin(), v.end(), std::begin(point.v));
    int index = 0;
    for (auto p : v) {
      u_char* dim_data = point.bytes + index * bytes_per_dim;
      sortable_bytes_encode(p, dim_data);
      index++;
    }
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
  void set_docid(uint32_t docid) { docid_ = docid; }

  T get(int dim) const {
    T d;
    sortable_bytes_decode(point.bytes + dim * bytes_per_dim, d);
    return d;
  }

  void set(T d, int dim) {
    u_char* dim_data = point.bytes + dim * bytes_per_dim;
    sortable_bytes_encode(d, dim_data);
  }

  /*
    T operator[](int index) {
      assert(index < D && index >= 0);
      T d;
      sortable_bytes_decode(point.bytes+index*bytes_per_dim,d);
      return d;
    }
  */
  Point operator-(const Point& p) {
    Point r;
    for (int i = 0; i < D; ++i) {
      T diff;
      // r[i] = point.v[i] - p.point.v[i];
      diff = get(i) - p.get(i);
      r.set(diff, i);
    }
    return r;
  }

  bool operator==(const Point& p) {
    return memcmp(p.point.bytes, point.bytes, bytes_size()) == 0;
  }

  bool operator!=(const Point& p) {
    return memcmp(p.point.bytes, point.bytes, bytes_size()) != 0;
  }

  bool operator<=(const Point& p) {
    for (int i = 0; i < dim; ++i) {
      if (p.get(i) < get(i)) return false;
    }
    return true;
  }

  bool operator>=(const Point& p) {
    for (int i = 0; i < dim; ++i) {
      if (p.get(i) > get(i)) return false;
    }
    return true;
  }

  bool operator<(const Point& p) {
    for (int i = 0; i < dim; ++i) {
      if (p.get(i) <= get(i)) return false;
    }
    return true;
  }

  bool operator>(const Point& p) {
    for (int i = 0; i < dim; ++i) {
      if (p.get(i) >= get(i)) return false;
    }
    return true;
  }

  int compare(const Point& p, int dim) {
    int index = dim * bytes_per_dim;
    return memcmp(point.bytes + index, p.point.bytes + index, bytes_per_dim);
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
    int index = (dim + 1) * bytes_per_dim + pos - 1;
    return point.bytes[index];
  }

  u_char* get_bytes_one_dim(int dim) {
    assert(dim < D);
    return point.bytes + bytes_per_dim * dim;
  }

  u_char* get_bytes(int dim) {
    assert(dim < D);
    return point.bytes + bytes_per_dim * dim;
  }

  size_t bytes_size() { return sizeof(T) * D; }

  int mismatch(const Point& p, int dim) {
    int index = dim * bytes_per_dim;
    int i = 0;
    for (; i < bytes_per_dim; ++i) {
      if (point.bytes[index + i] != p.point.bytes[index + i]) break;
    }
    return i;
  }

  void prefix_lens_per_dim(const Point& p, const std::vector<int>& ends,
                           std::vector<int>& prefix_lens) {
    assert(ends.size() == D && prefix_lens.size() == D);
    for (int i = 0; i < D; ++i) {
      int j = 0, end = ends[i];
      for (; j < end; ++j) {
        int index = (i)*bytes_per_dim + j;
        if (point.bytes[index] != p.point.bytes[index]) {
          break;
        }
      }
      prefix_lens[i] = j;
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const Point<T, D>& p) {
    os << '[';
    if (D == 1) {
      std::copy(std::begin(p.point.v), std::end(p.point.v),
                std::ostream_iterator<T>(os, ""));
      T d;
      sortable_bytes_decode(p.point.bytes, d);
      os << d;
    } else {
      for (int i = 0; i < D; ++i) {
        T d;
        int index = i * bytes_per_dim;
        sortable_bytes_decode(p.point.bytes + index, d);
        os << d;
        if (i != D - 1) os << ",";
      }
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
