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

    virtual int byte_at(int i,int pos) override{
      return points[i].get_byte(_sorted_dim,pos);
    }
    virtual ~MemoryPoints() = default;

    virtual void swap(int i, int j) {
      value_type tmp = points[i];
      points[i] = points[j];
      points[j] = tmp;
    }

    virtual bool compare(int i, int j) {
      return points[i][_sorted_dim] < points[j][_sorted_dim];
    }

    void minmax(int from, int to, value_type& min, value_type& max) override {
      if (from >= to) {
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


  private:
    std::vector<value_type> points;
    int offset;
    int _sorted_dim;
  };
}  // namespace yas