#pragma once

namespace yas {
class Sorter {
 public:
  static const int BINARY_SORT_THRESHOLD;
  Sorter() = default;
  virtual ~Sorter() = default;
  virtual void sort(int i, int j, void* cookie) = 0;
  virtual void select(int from, int to, int, void* cookie) = 0;
  virtual void swap(int i, int j) = 0;
  // return false when data[i]<data[j],or true
  virtual bool compare(int i, int j, void* cookie) = 0;  // less semantics
  virtual int max_length() = 0;
  virtual int byte_at(int i, int pos, void* cookie) = 0;

  virtual void set_pivot(int i) { _pivot_index = i; };

  virtual int compare_pivot(int i, void* cookie) {
    return compare(_pivot_index, i, cookie);
  }

  virtual void binary_sort(int from, int to, void* cookie) {
    for (int i = from + 1; i < to; ++i) {
      int loc = binary_search(from, i, i, cookie);
      for (int k = i; k > loc; k--) {
        swap(k, k - 1);
      }
    }
  }

  virtual void heap_sort(int from, int to, void* cookie) {
    for (int i = (from + to) / 2 - 1; i >= 0; --i) {
      heapify(i, to, cookie);
    }

    for (int i = to - 1; i >= 0; --i) {
      swap(i, from);
      heapify(from, i, cookie);
    }
  }

 private:
  // find the first pos which is greater than data[k] with binary search
  int binary_search(int i, int j, int k, void* cookie) {
    while (i < j) {
      int mid = (i + j) >> 1;
      if (compare(k, mid, cookie)) {
        j = mid - 1;
      } else {
        i = mid + 1;
      }
    }
    return compare(k, i, cookie) ? i : (i + 1);
  }

  void heapify(int from, int to, void* cookie) {
    int parent = from;
    int left = 2 * from + 1;
    int right = 2 * from + 2;

    while (left < to) {
      if (right < to && compare(left, right, cookie)) {
        left = right;
      }

      if (compare(left, parent, cookie))
        return;
      else {
        swap(parent, left);
        parent = left;
        left = parent * 2 + 1;
        right = parent * 2 + 2;
      }
    }
  }

 private:
  int _pivot_index;
};

}  // namespace yas