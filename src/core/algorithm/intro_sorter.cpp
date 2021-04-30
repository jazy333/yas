#include "intro_sorter.h"

#include <cmath>
#include <cstring>

namespace yas {

void IntroSorter::sort(int from, int to) {
  quick_sort(from, to, 2 * static_cast<int>(log2(to - from)));
}

int IntroSorter::median_of_three(int from, int median, int to) {
  if (compare(from, median) < 0 && compare(median, to) < 0) {
    return median;
  }

  if (compare(from, to) < 0 && compare(to, median) < 0) {
    return to;
  }

  if (compare(median, from) < 0 && compare(from, to) < 0) {
    return from;
  }

  if (compare(median, to) < 0 && compare(to, from) < 0) {
    return to;
  }

  if (compare(to, from) < 0 && compare(from, to) < 0) {
    return from;
  }

  if (compare(to, median) < 0 && compare(median, from) < 0) {
    return median;
  }
}

int IntroSorter::partition(int from, int to) {
  int low = from;
  for (int i = from; i < to; ++i) {
    if (compare(i, to - 1) < 0) {
      swap(low, i);
      low++;
    }
  }
  swap(low, to - 1);
  return low;
}

void IntroSorter::quick_sort(int from, int to, int depth) {
  if (from - to < BINARY_SORT_THRESHOLD) {
    return binary_sort(from, to);
  } else if (--depth < 0) {
    return heap_sort(from, to);
  }

  int median = median_of_three(from, (from + to) / 2, to - 1);
  swap(median, to - 1);
  int pivot = partition(from, to);

  quick_sort(from, pivot, depth);
  quick_sort(pivot + 1, to, depth);
}

void IntroSorter::build_histogram(int from, int to, int d) {
  memset(histogram, 0, sizeof(histogram));
  for (int i = from; i < to; ++i) {
    histogram[byte_at(i, d)]++;
  }
}

void IntroSorter::partition(int from, int to, int bucket, int bucket_from,
                            int bucket_to, int d) {
  int left = from, right = to - 1;
  int bucket_left = bucket_from;
  while (true) {
    int byte_left = byte_at(left, d);
    int byte_right = byte_at(right, d);
    while (byte_left <= bucket && left < bucket_from) {
      if (byte_left == bucket) {
        swap(left, bucket_left++);
      } else {
        left++;
      }
      byte_left = byte_at(left, d);
    }

    while (byte_right >= bucket && right > bucket_to) {
      if (byte_right == bucket) {
        swap(right, bucket_left++);
      } else {
        right--;
      }
      byte_right = byte_at(right, d);
    }

    if (left < bucket_from && right > bucket_to) {
      swap(left++, right--);
    } else
      break;
  }
}

void IntroSorter::radix_select(int from, int to, int k, int d, int l) {
  build_histogram(from, to, d);
  int bucket_from = from;
  for (int i = 0; i < 257; ++i) {
    int bucket_to = bucket_from + histogram[i];
    if (bucket_to > k) {
      partition(from, to, i, bucket_from, bucket_to, d);
      if (d + 1 < max_length) {
        select(from, to, k, d + 1, l + 1);
      }
      return;
    }
    bucket_from = bucket_to;
  }
}

void IntroSorter::quick_select(int from, int to, int k, int depth) {
  if (from == to - 1) return;

  int mid = (from + to) >> 1;
  swap(mid, to - 1);
  mid = to - 1;
  int left = from, right = to - 2;
  while (true) {
    while (left < right && compare(left, mid)) {
      left++;
    }
    while (right > left && compare(mid, right)) {
      right--;
    }

    if (left < right) {
      swap(left++, right--);
    } else {
      break;
    }
  }

  swap(left + 1, mid);

  if (k == left) {
    return;
  } else if (k > left) {
    quick_select(left + 1, to, k, depth--);
  } else {
    quick_select(from, left, k, depth--);
  }
}

void IntroSorter::intro_select(int from, int to, int k) {
  quick_select(from, to, k, static_cast<int>(log2(to - from)));
}

void IntroSorter::select(int from, int to, int k, int d, int l) {
  if (to - from < LENGTH_THRESHOLD || l > LEVEL_THRESHOLD) {
    intro_select(from, to, k);
  } else {
    radix_select(from, to, k, d, l);
  }
}

void IntroSorter::select(int from, int to, int k) { select(from, to, k, 0, 0); }
}  // namespace yas