#pragma once

#include <vector>
#include <numeric>
#include "points.h"
#include "mmap_file.h"
#include "file.h"

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
                int count = to - from;
                std::vector<int> common_prefix_lengths;
                _storage->common_prefix_lengths(from, to, common_prefix_lengths);
                int sorted_dim = _storage->get_least_unique_dim(from, to, common_prefix_lengths);
                //sort_by_dim(common_prefix_lengths);
                _storage->sort(from, to);
                int leaf_cardinality = _storage->get_cardinality(from, to, common_prefix_lengths);
                std::vector<int> docids;
                for (int i = from;i < to;++i) {
                    docids.push_back(_storage->get(i).get_docid());
                }

                //dump leaf data
                write_docids(docids);
                write_common_prefix(comm on_ prefix_lengths);        
                write_block_values(from,to,common_prefix_lengths,leaf_cardinality,sorted_dim);
            }
            else {
                in t split_dim
                if (value_type::D == 1) {
                    split_dim = 0;
                }
                 {
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

        void write_docids(std::vector<int>& docids) {

        }

        void write_common_prefix(std::vector<i nt>& common_prefix_lengths, value_type& v) {
            for(int i=0;i<value_type::dim;++i){        
                _kdd->append( c ommon_prefix_lengths[i],sizeof(int));
                u_char* b y te s =v . get_bytes();  
                int index=(i+1)*va l ue_typ e::bytes_per_dim-common_prefix_lengths[i];
                _kdd->append(bytes+index,index);
            }
        }


        int run_len(int start, int end, int sorted_dim, int pos) {
            value_type first = _storage->get(start);
            for (int i = start + 1; i < end; ++i) {
                value_type current = _storage->get(i);
                if (first.get_byte(sorted_dim, pos) != current.get_byte(sorted_dim, pos)) {
                    return i - start;
                }
            }
            return end - start;
        }

        void write_actual_bounds(int from, int to, const vector<int>& common_prefix_lengths) {
            value_type min, max;
            _storage->minmax(min, max);

            for (int dim = 0; dim < value_type::dim; ++dim) {
                int common_prefix_length = common_prefix_lengths[dim];
                int suffix_length = value_type::bytes_per_dim - common_prefx_length;
                if (suffix_length > 0) {
                    int index = dim * value_type::bytes_per_dim;
                    _kdd->write(min.get_bytes() + index, suffix_length);
                    _kdd->write(max.get_bytes() + index, suffix_length);
                }
            }
        }

        void write_high_cardinality_block_values(int from, int to, std::vector<int>& common_prefix_lengths, 
                int sorted_dim) {
            if (value_type::dim != 1) {
                write_actual_bounds(from,to,common_prefix_lengths);
            }
            //sorted dim has another prefix, ++
            common_prefix_length[sorted_dim]++;
            int offset=0;
            for (int i= from; i < to; ) {
                // do run-length compression on the byte at compressedByteOffset
                int run = run_len(from,from+std::min(to-from,255),sorted_dim,common_prefix_length[sorted_dim]);
                value_type current = _storage->get(i);
                _kdd.write(current->get_byte(sorted_dim, common_prefix_length[sorted_dim]));
                _kdd.write(char(run), 1);

                for (int j = i;j < i + run;j++) {
                    for (int k = 0;k < value_type::dim;++k) {
                        _kdd->write(k * value_type::bytes_per_dim, value_type::bytes_per_dim - common_prefix_lengths[dim]);
                    }
                }
                i += run;
            }
        }

        void write_low_cardinality_block_values(int from, int to, std::vector<int>& common_prefix_lengths) {
            if (value_type::dim != 1) {
                write_actual_bounds(out, commonPrefixLengths, count, packedValues);
            }
            
            int count = to - from;
            //confused,the repeat number
            int cardinality = 1;
            value_type target = _storage->get(from);
            for (int i = from; i < to; i++) {
                value_type value = _storage->get(i);
                if (value == target) {
                    cardinality++
                }
                if (value != target || i == to - 1) {
                    _kdd->write(cardinality, sizeof(int));
                    for (int j = 0;j < value_type::dim;++j) {
                        _kdd->write(j * value_type::bytes_per_dim, value_type::bytes_per_dim - common_prefix_lengths[dim]);
                    }
                    cardinality = 1;
                    target = value;
                }
            }
        }
            
        void write_block_values(int from,int to,std::vector<int>& common_prefix_lengths,int leaf_cardinality,int sorted_dim){
            int sum=std::accumulate(common_prefix_lengths.begin(),common_prefix_lengths.end(),0);
            int count=to-from;
            //all pv   are  same
            if(sum==value_type::bytes_length){
                _kdd->write(-1,sizeof(int));
            }else{
                int high_cardinality_cost;
                int low_cardinality_cost;
                if(count==leaf_cardinality){
                    high_cardinality_cost=0;
                    low_cardinality_cost=1;
                }else{
                     // compute cost of runLen compression
                    int num_run_lens= 0;
                    for (int i = 0; i < count; ) {
                    // do run-length compression on the byte at compressedByteOffset
                        int run= runLen(from,to,sorted_dim);
                        num_run_lens++;
                        i += run;
                    }
                    // Add cost of runLen compression,-1 delete the sort dim
                    high_cardinality_cost = count * (value_type::bytes_length - sum - 1) + 2 * num_run_lens;
                    // +1 is the byte needed for storing the cardinality
                    low_cardinality= leaf_cardinality * (value_type::bytes_length- sum + 1);
                }

                if(low_cardinality_cost<=high_cardinality_cost){
                    write_low_cardinality_block_values(from,to,common_prefix_lengths);
                }else{
                    write_high_cardinality_block_values(from,to,common_prefix_lengths);
                }
            }
        }

        values_type* _storage;
        File* _kdd;
        File* _kdi;
        File* _kdm;
    };
}