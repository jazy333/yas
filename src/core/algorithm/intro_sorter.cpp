#include "intro_sorter.h"

#include <cmath>
#include <cstring>

namespace yas {

void IntroSorter::sort(int from, int to,void* cookie) {
  quick_sort(from, to, 2 * static_cast<int>(log2(to - from)),cookie);
}

int IntroSorter::median_of_three(int from, int median, int to,void* cookie) {
  if (compare(from, median,cookie) < 0 && compare(median, to,cookie) < 0) {
    return median;
  }

  if (compare(from, to,cookie) < 0 && compare(to, median,cookie) < 0) {
    return to;
  }

  if (compare(median, from,cookie) < 0 && compare(from, to,cookie) < 0) {
    return from;
  }

  if (compare(median, to,cookie) < 0 && compare(to, from,cookie) < 0) {
    return to;
  }

  if (compare(to, from,cookie) < 0 && compare(from, to,cookie) < 0) {
    return from;
  }

  if (compare(to, median,cookie) < 0 && compare(median, from,cookie) < 0) {
    return median;
  }
  return 0;
}

int IntroSorter::partition(int from, int to,void* cookie) {
  int low = from;
  for (int i = from; i < to; ++i) {
    if (compare(i, to - 1,cookie) < 0) {
      swap(low, i);
      low++;
    }
  }
  swap(low, to - 1);
  return low;
}

void IntroSorter::quick_sort(int from, int to, int depth,void* cookie) {
  if (from - to < BINARY_SORT_THRESHOLD) {
    return binary_sort(from, to,cookie);
  } else if (--depth < 0) {
    return heap_sort(from, to,cookie);
  }

  int median = median_of_three(from, (from + to) / 2, to - 1,cookie);
  swap(median, to - 1);
  int pivot = partition(from, to,cookie);

  quick_sort(from, pivot, depth,cookie);
  quick_sort(pivot + 1, to, depth,cookie);
}

void IntroSorter::build_histogram(int from, int to, int d,void* cookie) {
  memset(histogram, 0, sizeof(histogram));
  for (int i = from; i < to; ++i) {
    histogram[byte_at(i, d,cookie)]++;
  }
}

void IntroSorter::partition(int from, int to, int bucket, int bucket_from,
                            int bucket_to, int d,void* cookie) {
  int left = from, right = to - 1;
  int bucket_left = bucket_from;
  while (true) {
    int byte_left = byte_at(left, d,cookie);
    int byte_right = byte_at(right, d,cookie);
    while (byte_left <= bucket && left < bucket_from) {
      if (byte_left == bucket) {
        swap(left, bucket_left++);
      } else {
        left++;
      }
      byte_left = byte_at(left, d,cookie);
    }

    while (byte_right >= bucket && right > bucket_to) {
      if (byte_right == bucket) {
        swap(right, bucket_left++);
      } else {
        right--;
      }
      byte_right = byte_at(right, d,cookie);
    }

    if (left < bucket_from && right > bucket_to) {
      swap(left++, right--);
    } else
      break;
  }
}

void IntroSorter::radix_select(int from, int to, int k, int d, int l,
                               void* cookie) {
  build_histogram(from, to, d,cookie);
  int bucket_from = from;
  for (int i = 0; i < 257; ++i) {
    int bucket_to = bucket_from + histogram[i];
    if (bucket_to > k) {
      partition(from, to, i, bucket_from, bucket_to,d, cookie);
      if (d + 1 < max_length) {
        select(from, to, k, d + 1, l + 1,   cookie);
      }
      return;
    }
    bucket_from = bucket_to;
  }
}

void IntroSorter::quick_select(int from, int to, int k, int depth, void* cookie) {
  if (from == to - 1) return;

  int mid = (from + to) >> 1;
  swap(mid, to - 1);
  mid = to - 1;
  int left = from, right = to - 2;
  while (true) {
    while (left < right && compare(left, mid,  cookie)) {
      left++;
    }
    while (right > left && compare(mid, right,  cookie)) {
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
    quick_select(left + 1, to, k, depth--,cookie);
  } else {
    quick_select(from, left, k, depth--, cookie);
  }
}

void IntroSorter::intro_select(int from, int to, int k, void* cookie) {
  quick_select(from, to, k, static_cast<int>(log2(to - from)), cookie);
}

void IntroSorter::select(int from, int to, int k, int d, int l,  void* cookie) {
  if (to - from < LENGTH_THRESHOLD || l > LEVEL_THRESHOLD) {
    intro_select(from, to, k,  cookie);
  } else {
    radix_select(from, to, k, d, l,  cookie);
  }
}

void IntroSorter::select(int from, int to, int k, void* cookie){
  select(from, to, k, 0, 0, cookie);
}
}  // namespace yas