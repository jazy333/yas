#pragma once

#include <memory>
#include <numeric>
#include <vector>

#include "file.h"
#include "memory_file.h"
#include "mmap_file.h"
#include "points.h"

namespace yas {
template <class T, int D>
class BkdTree {
 public:
  using values_type = Points<T, D>;
  using value_type = typename Points<T, D>::value_type;
  BkdTree() = default;

  virtual ~BkdTree() {}
#if 0
        struct  IntersectState{
            IntersectState():_level(1),_node_id(1){
            }
            int _level;
            int _node_id;
            int _split_dim;
            T _split_dim_value;
            int _num_leaves;
            std::vector<value_type> _split_values;
            std::vector<long> _left_fps;
            std::vector<int> _right_chilren;
            std::vector<bool> _neg;
            std::vector<int> _split_dims;
            value_type _low;
            value_type _high;
            bool is_leaf(){
                return _node_id>=_num_leaves;
            }
            void push_left(){
                _level++;
                node_id*=2;
                read_index_node(true);
            }
           
            void push_right(){
                _level++;
                _node_id=_node_id*2+1;
                read_index_node(false);
            }
            
             void pop(){
                 _node_id/=2;
                 _level--;
            }

            void read_index_node(bool is_left){
                _left_fps[level]=_left_fps[level-1];
                if(!is_left){
                    //TODO:read left most child
                    long fp;
                    _kdi->read_vint(fp);
                    _left_fps[level]=fp;
                }
                if(is_leaf()){
                    _split_dim=-1;
                }else{
                int code=0;
                _kdi->read_vlint(code);
                _split_dim=code%value_type::dim;
                _split_dims[level]=_split_dim;
                code/=value_type::dim;
                int prefix=code%(1+value_type::bytes_per_dim);
                int suffix=value_type::bytes_per_dim-prefix;
                _split_values[level]=_split_values[level-1];
                if(suffix>0){
                    int delta=code/(1+value_type::bytes_per_dim);
                    if(is_left){
                        delta=-delta;
                    }

                    int old= _split_values[level].get_byte(_split_dim,prefix);
                    uchar* dim_start= _split_values[level].get_values_one_dim(_split_dim);
                    _split_values[level].set_byte(_split_dim,(u_char)(prefix,old+delta));
                    dim_star[value_type::bytes_per_dim-prefix-1]=old+delta;
                    _kdi->read(dim_start,suffix);
                }

                int left_bytes=0;
                if(_node_id*2<_num_leaves){
                    _kdi->read_vint(left_bytes);
                }

                _right_chilren[level]=_kdi->size()+left_bytes;
                }
            }
        };
#endif

  int build(values_type* storage, File* kdd,
            std::vector<std::shared_ptr<MemoryFile>>& nodes, bool is_left,
            int leaves_offset, int num_leaves, long min_block_fp, int from,
            int to, value_type& min, value_type& max,
            std::vector<T>& split_dimension_values,
            std::vector<int>& split_dimensions, std::vector<int>& parent_splits,
            std::vector<off_t>& leaf_block_fps, value_type& last_split_value,
            std::vector<bool>& neg) {
    if (num_leaves == 1) {
      leaf_block_fps[leaves_offset] = kdd->size();
      int count = to - from;
      std::vector<int> common_prefix_lengths;
      storage->common_prefix_lengths(from, to, common_prefix_lengths);
      int sorted_dim =
          storage->get_least_unique_dim(from, to, common_prefix_lengths);
      storage->sort(from, to,&sorted_dim);
      int leaf_cardinality =
          storage->get_cardinality(from, to, common_prefix_lengths);
      std::vector<int> docids;
      for (int i = from; i < to; ++i) {
        docids.push_back(storage->get(i).get_docid());
      }

      // dump leaf data
      write_docids(kdd, docids);
      value_type start=storage->get(from);
      write_common_prefix(kdd, common_prefix_lengths,start);
      write_block_values(storage, kdd, from, to, common_prefix_lengths,
                         leaf_cardinality, sorted_dim);
      leaf_block_fps[leaves_offset + 1] = kdd->size();
      return 0;
    } else {
      /*left node format :|code|splitvalue|rightchild offset|
       * right node format :leftchild offset|code|splitvalue|rightchild offset|
       */
      int split_dim, parent_index_len = 0;
      std::shared_ptr<MemoryFile> parent (new MemoryFile());
      nodes.push_back(parent);
      if (value_type::dim == 1) {
        split_dim = 0;
      } else {
        value_type min, max;
        storage->minmax(from, to, min, max);
        split_dim = storage->get_split_dimension(min, max, parent_splits);
      }

      long left_block_fp;
      if (is_left) {
        left_block_fp = min_block_fp;
      } else {
        left_block_fp = leaf_block_fps[leaves_offset];
        long delta = left_block_fp - min_block_fp;
        // dump delta
        parent_index_len += parent->write_vint(delta);
      }

      int num_left_leaves = get_number_left_leaves(num_leaves);
      int num_right_leaves = num_leaves - num_left_leaves;
      int mid = from + num_left_leaves * 512;
      int prefix_len = min.mismatch(max, split_dim);
      storage->select(from, to, mid,&split_dim);
      int right = leaves_offset + num_left_leaves;
      int split = right - 1;
      split_dimensions[split] = split_dim;
      value_type split_value = storage->get(mid);
      int split_prefix = split_value.mismatch(last_split_value, split_dim);

      int first_diff_byte_delta = 0;
      if (split_prefix < value_type::bytes_per_dim) {
        // split_value.bytes
        first_diff_byte_delta =
            split_value.get_byte(split_dim, split_prefix) -
            last_split_value.get_byte(split_dim, split_prefix);
        if (neg[split_dim]) {
          first_diff_byte_delta = -first_diff_byte_delta;
        }
      }

      last_split_value[split_dim] = split_value[split_dim];

      int code = (first_diff_byte_delta * (1 + value_type::bytes_per_dim) +
                  split_prefix) *
                     value_type::dim +
                 split_dim;
      // dump code
      parent_index_len += parent->write_vint(code);

      int split_suffix = value_type::bytes_per_dim - split_prefix;
      if (split_suffix > 1) {
        // write split value
        parent_index_len += split_suffix;
        u_char* svb = split_value.get_bytes_one_dim(split_dim);
        parent->append(svb, split_suffix);
      }

      bool last_neg = neg[split_dim];
      neg[split_dim] = true;

      value_type min_split_value = min;
      value_type max_split_value = max;
      min_split_value[split_dim] = split_value[split_dim];
      max_split_value[split_dim] = split_value[split_dim];
      parent_splits[split_dim]++;
      int left_bytes =
          build(storage, kdd, nodes, true, leaves_offset, num_left_leaves,
                left_block_fp, from, mid, min, max_split_value,
                split_dimension_values, split_dimensions, parent_splits,
                leaf_block_fps, last_split_value, neg);
      neg[split_dim] = false;

      int right_bytes =
          build(storage, kdd, nodes, false, right, num_right_leaves,
                left_block_fp, mid, to, min_split_value, max,
                split_dimension_values, split_dimensions, parent_splits,
                leaf_block_fps, last_split_value, neg);
      // write left bytes
      if (num_left_leaves != 1) {
        parent_index_len += parent->write_vint(left_bytes);
      } else if (num_right_leaves == 1) {  // write right leaf fp
        long left_block_fp = leaf_block_fps[right];
        long delta = left_block_fp - min_block_fp;
        // dump delta
        parent_index_len += parent->write_vint(delta);
      }
      parent_splits[split_dim]--;
      neg[split_dim] = last_neg;
      return left_bytes + right_bytes + parent_index_len;
    }
  }

  void pack(values_type* storage, File* kdm, File* kdi, File* kdd) {
    int from = 0, to = storage->size();
    int num_leaves = (to - from) / 512 + 1;
    value_type min, max;
    std::vector<T> split_dimension_values(num_leaves, T());
    std::vector<int> split_dimensions(num_leaves, 0);
    std::vector<int> parent_splits(value_type::dim, 0);
    std::vector<off_t> leaf_block_fps(num_leaves + 1, 0);
    value_type last_split_value;
    std::vector<bool> neg(value_type::dim, false);
    std::vector<std::shared_ptr<MemoryFile>> nodes;
    build(storage, kdd, nodes, false, 0, num_leaves, 0, from, to, min, max,
          split_dimension_values, split_dimensions, parent_splits,
          leaf_block_fps, last_split_value, neg);
  }

#if 0
        void intersect(value_type low,value_type high){
            IntersectState is;
            is._low=low;
            is._high=high;
            do_intersect(_min,_max,is);
        }
#endif
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

  void write_docids(File* kdd, std::vector<int>& docids) {}

  void write_common_prefix(File* kdd, std::vector<int>& common_prefix_lengths,
                           value_type& v) {
    for (int i = 0; i < value_type::dim; ++i) {
      kdd->write_vint(common_prefix_lengths[i]);
      u_char* bytes = v.bytes();
      int index =
          (i + 1) * value_type::bytes_per_dim - common_prefix_lengths[i];
      kdd->append(bytes + index, index);
    }
  }

  int run_len(values_type* storage, int from, int to, int sorted_dim, int pos) {
    value_type first = storage->get(from);
    for (int i = from + 1; i < to; ++i) {
      value_type current = storage->get(i);
      if (first.get_byte(sorted_dim, pos) !=
          current.get_byte(sorted_dim, pos)) {
        return i - from;
      }
    }
    return to - from;
  }

  void write_minmax(values_type* storage, File* kdd, int from, int to,
                    const std::vector<int>& common_prefix_lengths) {
    value_type min, max;
    storage->minmax(min, max);

    for (int dim = 0; dim < value_type::dim; ++dim) {
      int common_prefix_length = common_prefix_lengths[dim];
      int suffix_length = value_type::bytes_per_dim - common_prefix_length;
      if (suffix_length > 0) {
        int index = dim * value_type::bytes_per_dim;
        kdd->append(min.bytes() + index, suffix_length);
        kdd->append(max.bytes() + index, suffix_length);
      }
    }
  }

  void write_high_cardinality_block_values(
      values_type* storage, File* kdd, int from, int to,
      std::vector<int>& common_prefix_lengths, int sorted_dim) {
    if (value_type::dim != 1) {
      write_minmax(storage, kdd, from, to, common_prefix_lengths);
    }
    // sorted dim has another prefix, ++
    common_prefix_lengths[sorted_dim]++;
    int offset = 0;
    for (int i = from; i < to;) {
      // do run-length compression
      char run = run_len(storage, from, from + std::min(to - from, 255), sorted_dim,
                         common_prefix_lengths[sorted_dim]);
      value_type current = storage->get(i);
      u_char next =
          current.get_byte(sorted_dim, common_prefix_lengths[sorted_dim]);
      kdd->append(&next, 1);
      kdd->append(&run, 1);

      for (int j = i; j < i + run; j++) {
        for (int k = 0; k < value_type::dim; ++k) {
          value_type one = storage->get(j);
          u_char* dim_data = one.get_bytes_one_dim(k);
          kdd->append(dim_data,
                      value_type::bytes_per_dim - common_prefix_lengths[k]);
        }
      }
      i += run;
    }
  }

  void write_low_cardinality_block_values(
      values_type* storage, File* kdd, int from, int to,
      std::vector<int>& common_prefix_lengths) {
    if (value_type::dim != 1) {
      write_minmax(storage, kdd, from, to, common_prefix_lengths);
    }

    int count = to - from;
    // confused,the repeat number
    int cardinality = 1;
    value_type target = storage->get(from);
    for (int i = from + 1; i < to; i++) {
      value_type value = storage->get(i);
      if (value == target) {
        cardinality++;
      }
      if (value != target || i == to - 1) {
        kdd->write_vint(cardinality);
        for (int j = 0; j < value_type::dim; ++j) {
          u_char* dim_data = target.get_bytes_one_dim(j);
          kdd->append(dim_data,
                      value_type::bytes_per_dim - common_prefix_lengths[j]);
        }
        cardinality = 1;
        target = value;
      }
    }
  }

  void write_block_values(values_type* storage, File* kdd, int from, int to,
                          std::vector<int>& common_prefix_lengths,
                          int leaf_cardinality, int sorted_dim) {
    int sum = std::accumulate(common_prefix_lengths.begin(),
                              common_prefix_lengths.end(), 0);
    int count = to - from;
    // all pv are  same
    if (sum == value_type::bytes_length) {
      char magic = -1;
      kdd->append(&magic, 1);
    } else {
      int high_cardinality_cost;
      int low_cardinality_cost;
      if (count == leaf_cardinality) {
        high_cardinality_cost = 0;
        low_cardinality_cost = 1;
      } else {
        // compute cost of run_len compression
        int num_run_lens = 0;
        for (int i = 0; i < count;) {
          // do run-length compression
          int run = run_len(storage, from, to, sorted_dim,common_prefix_lengths[sorted_dim]);
          num_run_lens++;
          i += run;
        }
        // Add cost of runLen compression,-1 delete the sort dim
        high_cardinality_cost =
            count * (value_type::bytes_length - sum - 1) + 2 * num_run_lens;
        // +1 is the byte needed for storing the cardinality
        low_cardinality_cost =
            leaf_cardinality * (value_type::bytes_length - sum + 1);
      }

      if (low_cardinality_cost <= high_cardinality_cost) {
        char magic = -2;
        kdd->append(&magic, 1);
        write_low_cardinality_block_values(storage, kdd, from, to,
                                           common_prefix_lengths);
      } else {
        char magic = sorted_dim;
        kdd->append(&magic, 1);
        write_high_cardinality_block_values(storage, kdd, from, to,
                                            common_prefix_lengths, sorted_dim);
      }
    }
  }
#if 0
        void read_minmax(std::vector<int>& common_prefixs,value_type& min,value_type& max){
            for (int dim = 0; dim < value_type::dim; dim++) {
                int prefix = common_prefixs[dim];
                _kdd->read(min, dim * value_type::bytes_per_dim + prefix, value_type::bytes_per_dim - prefix);
                _kdd->read(max, dim * value_type::bytes_per_dim + prefix, value_type::bytes_per_dim - prefix);
            }
        }

        void read_docids(long fp){
            _kdd->seek(fp);
            

        }

        void read_low_cardinality_docvalues( std::vector<int>& common_prefixes,int count){
            int i;
            for (i = 0; i < count;) {
                int length=0; 
                _kdd->read_vint(length);
                for(int dim = 0; dim < value_tyep::dim; dim++) {
                    int prefix = common_prefixes[dim];
                    in.readBytes(scratchPackedValue, dim*config.bytesPerDim + prefix, config.bytesPerDim - prefix);
                }
                scratchIterator.reset(i, length);
                visitor.visit(scratchIterator, scratchPackedValue);
                i += length;
            }
   
        }

        void read_high_cardinaliy_docvalues( std::vector<int>& common_prefixes,int sorted_dim,int count){
            // the byte at `compressedByteOffset` is compressed using run-length compression,
            // other suffix bytes are stored verbatim
            int offset = sorted_dim * value_type::bytes_per_dim + common_prefixes[sorted_dim];
            common_prefixes[sorted_dim]++;
            int i;
            for (i = 0; i < count; ) {
                scratchPackedValue[compressedByteOffset] = in.readByte();
                char run_len;
                 _kdd->read(&run_len,1);
                for (int j = 0; j < run_len; ++j) {
                    for(int dim = 0; dim < value_type::dim; dim++) {
                        int prefix = common_prefixes[sorted_dim];
                        in.readBytes(scratchPackedValue, dim*config.bytesPerDim + prefix, config.bytesPerDim - prefix);
                    }
                    visitor.visit(scratchIterator.docIDs[i+j], scratchPackedValue);
                }
                i += run_len;
            }
    
        }

        void read_docvalues(){
            std::vector<int> common_prefixes;
            read_common_prefixs(common_prefixes);
            int count==0;
            _kdd->read_vint(count);
            int sorted_dim=0;
            _kdd->read_vint(sorete_dim);
            value_type min,max;
            switch (sorted_dim){
            case  -1:{
                break;
            }
            case -2:{
                read_minmax(common_prefixes,min,max);
                read_low_cardinality_docvalues(common_prefixes);
                break;
            }
            default:
                read_minmax(common_prefixes,min,max);
                read_high_cardinaliy_dovvalues(common_prefixes,sorted_dim);
                break;
            }
        }

        void add_all(IntersectStat& is){
            if(is.is_leaf()){
                read_docids(is._left_fps[is._level]);
            }else{
                is.push_left();
                add_all(is);
                is.pop();
                is.push_right()
                is.pop();
            }
        }

        void do_intersect(value_type& min,value_type& max,IntersectState& is){
            if(min>is._high||max<is._low){
                return;
            }else if(min>is._low &&max<is._high){
                add_all(is);
            }else if(is.is_leaf()){

            }else{
                int split_dim=is._split_dim;
                T split_dim_value=is._split_dim_value;
                is.push_left();
                value_type split_value=high;
                split_value[split_dim]=split_dim_value;
                do_intersect(low,split_value,is);
                is.pop();
                split_value=low;
                split_value[split_dim]=split_dim_value;
                is.push_right();
                do_intersect(split_value,high,is);
                is.pop();
            }
        }
#endif
  int _depth;
  int _num_leaves;
  value_type _min;
  value_type _max;
};
}  // namespace yas