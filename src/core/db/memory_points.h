#pragma once
#include <vector>

#include "points.h"

namespace yas {
  template <class T, int D>
  class MemoryPoints : public Points<T, D> {
  public:
    using value_type = Point<T, D>;
    MemoryPoints() : offset(0) {}
    void write(const value_type& value) override {
      points.push_back(value);
    }
    bool next() override {
      return ++offset >= points.size() ? false : true;
    }
    size_t size() override {
      return points.size();
    }
    value_type read() override {
      return points[offset];
    }
    virtual ~MemoryPoints() = default;


    void minmax(int from, int to, value_type& min, value_type& max) override {
      if (from >= to) {
        return;
      }

      min = points[from];
      max = points[from];
      int dim = min.dim();
      for (int i = from + 1;i < to;++i) {
        value_type v = points[i];
        for (int j = 0;j < dim;++j) {
          if (min[j] > v[j]) {
            min[j] = v[j];
          }

          if (max[j] < v[j]) {
            max[j] = v[j];
          }
        }
      }

    }
    void minmax(value_type& min, value_type& max) override {
      return minmax(0, points.size(), min, max);
    }
    //int get_split_dimension(value_type& min, value_type& max);

  private:
    std::vector<value_type> points;
    int offset;
  };
}  // namespace yas