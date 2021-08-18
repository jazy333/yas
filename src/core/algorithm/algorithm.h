#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace yas {
int levenshtein_distance(const char* a, const char* b);
int levenshtein_distance(const std::string& a, const std::string& b);
void permute(const std::vector<int>&, std::vector<std::vector<int>>&);
template <class ForwardIt>
ForwardIt max_element(ForwardIt first, ForwardIt last) {
  if (first == last) return last;

  ForwardIt largest = first;
  ++first;
  for (; first != last; ++first) {
    if (*largest < *first) {
      largest = first;
    }
  }
  return largest;
}

template <class ForwardIt, class T, class Compare>
ForwardIt exponential_search(ForwardIt first, ForwardIt last, const T& value,
                             Compare comp) {
  int bound = 1;
  ForwardIt pre;
  do {
    pre = first;
    std::advance(first, bound);
    bound <<= 1;
  } while (first < last && comp(*first, value));
  first = std::lower_bound(pre, first < last ? first : last, value, comp);
  return first;
}

}  // namespace yas