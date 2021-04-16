#pragma once

namespace yas {
    class Sorter {
    public:
        static const int BINARY_SORT_THRESHOLD;
        Sorter() = default;
        virtual ~Sorter() = default;
        virtual void sort(int i, int j) = 0;
        virtual void swap(int i, int j) = 0;
        virtual int compare(int i, int j) = 0;

        virtual void set_pivot(int i) {
            _pivot_index = i;
        };

        virtual int compare_pivot(int i) {
            return compare(_pivot_index, i);
        }

        virtual void binary_sort(int from, int to) {
            for (int i = from + 1;i < to;++i) {
                int j = i - 1;
                int loc = binary_search(from, i, i);
                for (int k = i;k > loc;k--) {
                    swap(k, k - 1);
                }
            }
        }

        virtual void heap_sort(int from, int to) {
            for (int i = (from + to) / 2 - 1;i >= 0;--i) {
                heapify(i, to);
            }

            for (int i = to - 1;i >= 0;--i) {
                swap(i, from);
                heapify(from, i);
            }
        }
    private:
        int binary_search(int i, int j, int k) {
            while (i < j) {
                int mid = (i + j) >> 1;
                if (compare(k, mid) < 0) {
                    j = mid - 1;
                }
                else {
                    i = mid + 1;
                }
            }
            return compare(k, i) > 0 ? (i + 1) : i;

        }

        void heapify(int from, int to) {
            int parent=from;
            int left = 2 * from + 1;
            int right = 2 * from + 2;


            while (left < to) {

                if (right < to && compare(left, right) < 0) {
                     left= right;
                }

                if (compare(parent,left) > 0)
                    return; 
                else {
                    swap(parent, left);
                    parent=left;
                    left = parent * 2;
                    right = left + 1;
                }
           }
        }
    private:
        int _pivot_index;
    };
    const int Sorter::BINARY_SORT_THRESHOLD = 16;
}//namespace yas