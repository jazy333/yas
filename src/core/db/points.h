#pragma once
#include "point.h"
#include <vector>
#include <bitset>
#include <limits>

namespace yas {
  template <class T, int D>
  class Points {
    class points_iterator;
  public:
    using value_type = Point<T, D>;
    using iterator = points_iterator;

    virtual bool next() = 0;
    virtual size_t size() = 0;
    virtual value_type read() = 0;
    virtual void write(const value_type& v) = 0;
    virtual value_type get(int index) = 0;
    virtual void minmax(int from, int to, value_type& min, value_type& max) = 0;
    virtual void minmax(value_type& min, value_type& max) = 0;
    virtual ~Points() = default;

    void common_prefix_lengths(int from, int to, std::vector<int>& prefix_lengths) {
      // Compute common prefixes

      prefix_lengths.resize(value_type::dim, value_type::bytes_per_dim);
      value_type first = get(from);
      for (int i = from + 1;i < to;++i) {
        value_type current = get(i);
        first.prefix_lens_per_dim(current, prefix_lengths, prefix_lengths);
      }

    }

    void common_prefix_lengths(std::vector<int>& prefix_lengths) {
      common_prefix_lengths(0, size(), prefix_lengths);
    }


    int get_least_uniqe_dim(int from, int to, std::vector<int>& prefix_lengths) {
      // Find the dimension that has the least number of unique bytes at commonPrefixLengths[dim]

      std::vector<std::bitset<256>> bs(value_type::dim);
      for (int i = 0;i < value_type::dim;++i) {
        if (prefix_lengths[i] == value_type::bytes_per_dim) {
          bs[i].flip();
        }
      }

      for (int i = from;i < to;++i) {
        for (int j = 0;j < value_type::dim;++j) {
          if (prefix_lengths[j] != value_type::bytes_per_dim) {
            u_char b = get(i).get_byte(j, prefix_lengths[j]);
            bs[j][b] = true;
          }
        }
      }

      int sorted_dim = 0;
      int min_cardinality = std::numeric_limits<int>::max();
      for (int i = 0;i < value_type::dim;++i) {
        int cardinality = bs[i].count();
        if (cardinality < min_cardinality) {
          sorted_dim = i;
          min_cardinality = cardinality;
        }
      }

      return sorted_dim;

    }


    int get_least_uniqe_dim(std::vector<int>& prefix_lengths) {
      return get_least_uniqe_dim(0, size(), prefix_lengths);
    }

    int get_cardinality(int from, int to, std::vector<int>& prefix_lengths) {
      value_type pre = get(from);
      int cardinality = 1;
      for (int i = from + 1;i < to;++i) {
        value_type current = get(i);
        if (pre == current) {
          continue;
        }
        else {
          cardinality++;
          pre = current;
        }
      }

      return cardinality;
    }


    int get_cardinality(std::vector<int>& prefix_lengths) {
     return get_cardinality(0, size(), prefix_lengths);
    }

    iterator begin()  {
      return points_iterator(this,0);
    }

    iterator end()  {
      size_t last;
      return points_iterator(this,static_cast<int>(last));
    }

  private:
    class points_iterator : public std::iterator<std::random_access_iterator_tag, value_type>
    {
    public:
      using difference_type = typename std::iterator<std::random_access_iterator_tag, value_type>::difference_type;

      points_iterator() : _cur(0) {}
      points_iterator(Points*,int cur) :_cur(cur) {}
      points_iterator(const points_iterator& rhs) : _cur(rhs._cur) {}
      inline points_iterator& operator=(const points_iterator& rhs) { _cur = rhs._cur; return *this; }
      inline points_iterator& operator+=(difference_type rhs) { _cur += rhs; return *this; }
      inline points_iterator& operator-=(difference_type rhs) { _cur -= rhs; return *this; }
      //inline value_type& operator*() const { /*return parent->get(_cur);*/ }
      inline value_type* operator->() const { return parent->get(_cur); }
      inline value_type& operator[](difference_type rhs) const { return get(_cur); }

      inline points_iterator& operator++() { ++_cur; return *this; }
      inline points_iterator& operator--() { --_cur; return *this; }
      //inline points_iterator operator++(int) const { points_iterator tmp(*this); ++_cur; return tmp; }
      //inline points_iterator operator--(int) const { points_iterator tmp(*this); --_cur; return tmp; }
      inline difference_type operator-(const points_iterator& rhs) const { return points_iterator(_cur - rhs.cur); }
      inline points_iterator operator+(difference_type rhs) const { return points_iterator(_cur + rhs); }
      inline points_iterator operator-(difference_type rhs) const { return points_iterator(_cur - rhs); }
      friend inline points_iterator operator+(difference_type lhs, const points_iterator& rhs) { return points_iterator(lhs + rhs._cur); }
      friend inline points_iterator operator-(difference_type lhs, const points_iterator& rhs) { return points_iterator(lhs - rhs._cur); }

      inline bool operator==(const points_iterator& rhs) const { return _cur == rhs._cur; }
      inline bool operator!=(const points_iterator& rhs) const { return _cur != rhs._cur; }
      inline bool operator>(const points_iterator& rhs) const { return _cur > rhs._cur; }
      inline bool operator<(const points_iterator& rhs) const { return _cur < rhs._cur; }
      inline bool operator>=(const points_iterator& rhs) const { return _cur >= rhs._cur; }
      inline bool operator<=(const points_iterator& rhs) const { return _cur <= rhs._cur; }
    private:
      Points* parent;
      int _cur;
    };
  };
}  // namespace yas