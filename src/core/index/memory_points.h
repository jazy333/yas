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
    if (points.size() == 0) {
      min_ = value;
      max_ = value;
    } else {
      for (int i = 0; i < D; ++i) {
        if (min_.get(i) > value.get(i)) {
          min_.set(value.get(i), i);
        }
        if (max_.get(i) < value.get(i)) {
          max_.set(value.get(i), i);
        }
      }
    }
    points.push_back(value);
  }
  bool next() override { return ++offset >= points.size() ? false : true; }
  size_t size() override { return points.size(); }
  value_type read() override { return points[offset]; }

  virtual value_type get(int index) override {
    assert(index >= 0 && index < points.size());
    return points[index];
  }

  virtual int byte_at(int i, int pos, void* cookie) override {
    int sorted_dim = *(static_cast<int*>(cookie));
    return points[i].get_byte(sorted_dim, pos);
  }
  virtual ~MemoryPoints() = default;

  virtual void swap(int i, int j) {
    value_type tmp = points[i];
    points[i] = points[j];
    points[j] = tmp;
  }

  virtual bool compare(int i, int j, void* cookie) {
    int sorted_dim = *(static_cast<int*>(cookie));
    return points[i].get(sorted_dim) < points[j].get(sorted_dim);
  }

  void minmax(int from, int to, value_type& min, value_type& max) override {
    if (from >= to) {
      return;
    }

    min = points[from];
    max = points[from];
    int dim = value_type::dim;
    for (int i = from + 1; i < to; ++i) {
      value_type v = points[i];
      for (int j = 0; j < dim; ++j) {
        if (min.compare(v, j) > 0) {
          min.set(v.get(j), j);
        }

        if (max.compare(v, j) < 0) {
          max.set(v.get(j), j);
        }
      }
    }
  }

  void minmax(value_type& min, value_type& max) override {
    // return minmax(0, points.size(), min, max);
    min = min_;
    max = max_;
  }

 private:
  std::vector<value_type> points;
  value_type min_;
  value_type max_;
  int offset;
};
}  // namespace yas