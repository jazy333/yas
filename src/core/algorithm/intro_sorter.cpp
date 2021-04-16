#include "intro_sorter.h"
#include <cmath>

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
        for (int i = from;i < to;++i) {
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
        }
        else if (--depth < 0) {
            return heap_sort(from, to);
        }

        int median = median_of_three(from, (from + to) / 2, to - 1);
        swap(median, to - 1);
        int pivot = partition(from, to);

        quick_sort(from, pivot, depth);
        quick_sort(pivot + 1, to, depth);
    }
}