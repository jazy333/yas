#pragma once
#include "sorter.h"

namespace yas {
class IntroSorter : public Sorter {
 public:
  IntroSorter() = default;
  ~IntroSorter() = default;
  void sort(int from, int to, void* cookie) override;
  void select(int from, int to, int k, void* cookie) override;

 private:
  static const int LEVEL_THRESHOLD;
  static const int LENGTH_THRESHOLD;
  int partition(int from, int to, void* cookie);
  int median_of_three(int from, int mid, int to, void* cookie);
  void quick_sort(int from, int to, int depth, void* cookie);
  void build_histogram(int from, int to, int d, void* cookie);
  void partition(int from, int to, int bucket, int bucket_from, int bucket_to,
                 int d, void* cookie);
  void radix_select(int from, int to, int k, int d, int l, void* cookie);
  void quick_select(int from, int to, int k, int depth, void* cookie);
  void intro_select(int from, int to, int k, void* cookie);
  void select(int from, int to, int k, int d, int l, void* cookie);
  int histogram[257];
  int max_length;
};

const int IntroSorter::LEVEL_THRESHOLD = 8;
const int IntroSorter::LENGTH_THRESHOLD = 100;
}  // namespace yas