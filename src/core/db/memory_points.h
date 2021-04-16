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

    virtual value_type get(int index) override {
      assert(index >= 0 && index < points.size());
      return points[index];
    }
    virtual ~MemoryPoints() = default;


    void minmax(int from, int to, value_type& min, value_type& max) override {
      if (from > to) {
        return;
      }

      min = points[from];
      max = points[from];
      int dim = value_type::dim;
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

    int get_split_dimension(value_type& min, value_type& max, std::vector<int>& parent_splits) {
      int max_splits = 0;
      for (auto splits : parent_splits) {
        if (splits > max_splits) {
          max_splits = splits;
        }
      }

      int dim = min.dim();

      for (int i = 0;i < dim;++i) {
        if (parent_splits[i] < max_splits / 2 && min[i] != max[i])
          return i;
      }

      int split_dim = -1;
      T max_dim_diff;
      value_type diff = max - min;
      for (int i = 0;i < dim;++i) {
        if (diff[i] > max_dim_diff) {
          max_dim_diff = diff[i];
          split_dim = i;
        }
      }

      return split_dim;
    }

  private:
    std::vector<value_type> points;
    int offset;
  };
}  // namespace yas