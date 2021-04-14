#pragma once
#include "point.h"

namespace yas {
  template <class T, int D>
  class Points {
  public:
    using value_type = Point<T, D>;
    virtual bool next() = 0;
    virtual size_t size() = 0;
    virtual value_type read() = 0;
    virtual void write(const value_type& v)=0;
    virtual void minmax(int from, int to, value_type& min, value_type& max) = 0;
    virtual void minmax(value_type& min, value_type& max) = 0;
    virtual ~Points() = default;
  };
}  // namespace yas