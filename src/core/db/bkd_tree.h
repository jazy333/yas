#pragma once

#include <vector>
#include "points.h"


namespace yas {
    template<class T, int D>
    class BkdTree {
    public:
        using values_type = Points<T, D>;
        using value_type = typename Points<T, D>::value_type;
        BkdTree(values_type* values) :_storage(values) {
        }

        virtual ~BkdTree() {
        }

        int build(bool is_left, int leaves_offset, int num_leaves, int from, int to, value_type& min, value_type& max,
            std::vector<T>& split_dimension_values, std::vector<int>& split_dimensions, std::vector<int>& parent_splits,
            std::vector<off_t>& leaf_block_fps, value_type& last_split_value, std::vector<bool>& neg) {
            if (num_leaves == 1) {
                int count = from - to;
                std::vector<int> common_prefix_lengths;
                _storage->common_prefix_lengths(from, to, common_prefix_lengths);
                int sorted_dim = _storage->get_least_unique_dim(from, to, common_prefix_lengths);
                //sort_by_dim(common_prefix_lengths);
                _storage->sort(from, to);
                int leaf_cardinality = _storage->get_cardinality(from, to, common_prefix_lengths);
            }
            else {
                int split_dim;
                if (value_type::D == 1) {
                    split_dim = 0;
                }
                else {
                    _storage->minmax(from, to, min, max);
                    split_dim = _storage->get_split_dimension(min, max, parent_splits);
                }

                int num_left_leaves = get_number_left_leaves(num_leaves);
                int mid = from + num_left_leaves * 512;
                int prefix_len = min.mismatch(max, split_dim);
                _storage->select(from, to, mid);
                int right = leaves_offset + num_left_leaves;
                int split = right - 1;
                split_dimensions[split] = split_dim;
                value_type split_value = _storage->get(mid);
                int split_prefix = split_value.mismatch(last_split_value, split_dim);

                int first_diff_byte_delta = 0;
                if (split_prefix < value_type::bytes_per_dim) {
                    //split_value.bytes
                    first_diff_byte_delta = split_value.get_byte(split_dim, split_prefix) - last_split_value.get_byte(split_dim, split_prefix);
                    if (neg[split_dim]) {
                        first_diff_byte_delta = -first_diff_byte_delta;
                    }
                }

                last_split_value[split_dim] = split_value[split_dim];

                int code = (first_diff_byte_delta * (1 + value_type::bytes_per_dim) + split_prefix) * value_type::D + split_dim;
                int split_suffix = value_type::bytes_per_dim - split_prefix;
                if (split_suffix > 1) {
                    //TODO:write split value
                }


                bool last_neg = neg[split_dim];
                neg[split_dim] = true;

                value_type min_split_value = min;
                value_type max_split_value = max;
                min_split_value[split_dim] = split_value[split_dim];
                max_split_value[split_dim] = split_value[split_dim];
                parent_splits[split_dim]++;
                int left_bytes = build(true, leaves_offset, num_left_leaves, from, mid, min, max_split_value, split_dimension_values, split_dimensions, parent_splits, leaf_block_fps, last_split_value, neg);
                //TODO:write left bytes

                neg[split_dim] = false;
                int right_bytes = build(false, right, num_leaves - num_left_leaves, mid, to, min_split_value, max, split_dimension_values, split_dimensions, parent_splits, leaf_block_fps, last_split_value, neg);
                parent_splits[split_dim]--;
                neg[split_dim] = last_neg;
                return left_bytes + right_bytes;
            }
        }
    private:
        // for construct complete binary tree
        int get_number_left_leaves(int number) {
            int last_level = 31 - __builtin_clz(number);
            int full_level = 1 << last_level;
            int num_left = full_level / 2;
            int unbalanced = number - full_level;
            num_left += std::min(num_left, unbalanced);
            return num_left;
        }
        values_type* _storage;
    };
}