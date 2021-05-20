#pragma once

#include <memory>
#include <numeric>
#include <vector>
#include <cmath>

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

  struct IntersectState {
    IntersectState(int num_leaves, loff_t off, File* kdi,File* kdd)
        : level_(1),
          node_id_(1),
          _split_dim(0),
          _num_leaves(num_leaves),
          index_init_fp_(off),
          index_cur_fp_(off),
          kdi_(kdi),
          kdd_(kdd) {
      _left_fps.resize(tree_depth(num_leaves)+1, 0);
      _right_children.resize(tree_depth(num_leaves)+1,0);
      _split_dims.resize(tree_depth(num_leaves)+1,0);
      _split_values.resize(tree_depth(num_leaves)+1,value_type());
      neg_.resize(tree_depth(num_leaves)+1,std::vector<bool>(value_type::dim,false));
      read_index_node(false);
    }
    int level_;
    int node_id_;
    int _split_dim;
    T _split_dim_value;
    int _num_leaves;
    loff_t index_init_fp_;
    loff_t index_cur_fp_;
    File* kdi_;
    File* kdd_;
    std::vector<value_type> _split_values;
    std::vector<long> _left_fps;
    std::vector<int> _right_children;
    std::vector<std::vector<bool>> neg_;
    std::vector<int> _split_dims;
    std::vector<int> docids_;
    std::vector<value_type> docvalues_;
    value_type _low;
    value_type _high;
    bool is_leaf() { return node_id_ >= _num_leaves; }
    void push_left() {
      level_++;
      node_id_ *= 2;
      read_index_node(true);
    }

    void push_right() {
      index_cur_fp_ = _right_children[level_];
      level_++;
      node_id_ = node_id_ * 2 + 1;
      read_index_node(false);
    }

    void pop() {
      node_id_ /= 2;
      level_--;
      _split_dim=_split_dims[level_];
    }

    int tree_depth(int num_leaves){
      return log2(num_leaves)+2;
    }

    int relation(value_type& min,value_type& max){
      bool include=true;
      for(int i=0;i<value_type::dim;++i){
        //exclude
        if(_low.get(i)>max.get(i)||_high.get(i)<min.get(i)){
          return 0;
        }else if(!(_low.get(i)<=min.get(i)&&_high.get(i)>=max.get(i))){
          include=false;
        }
      }
      if(include)
        return 1;
      else
        //cross
        return 2;
    }

    int read_index_node(bool is_left) {
      _left_fps[level_] = _left_fps[level_ - 1];
      neg_[level_]=neg_[level_-1];
      neg_[level_][_split_dim]=is_left;
      if (!is_left) {
        //read right child
        long fp;
        int ret = kdi_->read_vint(index_cur_fp_, fp);
        index_cur_fp_ += ret;
        _left_fps[level_] = fp;
      }

      if (is_leaf()) {
        _split_dim = -1;
      } else {
        int code = 0;
        int ret = kdi_->read_vint(index_cur_fp_, code);
        index_cur_fp_ += ret;
        _split_dim = code % value_type::dim;
          
        code /= value_type::dim;
        int prefix = code % (1 + value_type::bytes_per_dim);
        int suffix = value_type::bytes_per_dim - prefix;
        _split_values[level_] = _split_values[level_ - 1];
        
        if (suffix > 0) {
          int delta = code / (1 + value_type::bytes_per_dim);
          if (neg_[level_][_split_dim]) {
            delta = -delta;
          }
          int old = _split_values[level_].get_byte(_split_dim, prefix);
          u_char* dim_start = _split_values[level_].get_bytes(_split_dim);
          dim_start[prefix] = old + delta;
          int ret =
              kdi_->read(index_cur_fp_, dim_start + prefix + 1, suffix - 1);
          index_cur_fp_ += ret;
        }

        int left_bytes = 0;
        if (node_id_ * 2 < _num_leaves) {
          int ret = kdi_->read_vint(index_cur_fp_, left_bytes);
          index_cur_fp_ += ret;
        }
        _right_children[level_] = index_cur_fp_ + left_bytes;
      }
      return 0;
    }
  };

  int build(values_type* storage, File* kdd,
            std::vector<std::shared_ptr<File>>& nodes, bool is_left,
            int leaves_offset, int num_leaves, long min_block_fp, int from,
            int to, value_type& min, value_type& max,
            std::vector<int>& parent_splits, std::vector<off_t>& leaf_block_fps,
            value_type& last_split_value, std::vector<bool>& neg) {
    if (num_leaves == 1) {
      leaf_block_fps[leaves_offset] = kdd->size();
      int count = to - from;
      std::vector<int> common_prefix_lengths;
      storage->common_prefix_lengths(from, to, common_prefix_lengths);
      int sorted_dim =
          storage->get_least_unique_dim(from, to, common_prefix_lengths);
      storage->sort(from, to, &sorted_dim);
      int leaf_cardinality =
          storage->get_cardinality(from, to, common_prefix_lengths);
      std::vector<int> docids;
      for (int i = from; i < to; ++i) {
        docids.push_back(storage->get(i).get_docid());
      }

      // dump leaf data
      write_docids(kdd, docids);
      value_type start = storage->get(from);
      write_common_prefix(kdd, common_prefix_lengths, start);
      write_block_values(storage, kdd, from, to, common_prefix_lengths,
                         leaf_cardinality, sorted_dim);
      leaf_block_fps[leaves_offset + 1] = kdd->size();
      return 0;
    } else {
      /*left node format :|code|splitvalue|rightchild offset|
       *right node format :leftchild offset|code|splitvalue|rightchild offset|
       */
      int split_dim, parent_index_len = 0;
      std::shared_ptr<MemoryFile> parent(new MemoryFile());
      nodes.push_back(parent);
      if (value_type::dim == 1) {
        split_dim = 0;
      } else {
        // value_type min, max;
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
      int mid = from + num_left_leaves * 2;
      int prefix_len = min.mismatch(max, split_dim);
      storage->select(from, to, mid, &split_dim);
      int right = leaves_offset + num_left_leaves;
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

      last_split_value.set(split_value.get(split_dim), split_dim);

      int code = (first_diff_byte_delta * (1 + value_type::bytes_per_dim) +
                  split_prefix) *
                     value_type::dim +
                 split_dim;
      // dump code
      parent_index_len += parent->write_vint(code);

      int split_suffix = value_type::bytes_per_dim - split_prefix;
      if (split_suffix > 1) {
        // write split value
        u_char* svb = split_value.get_bytes_one_dim(split_dim);
        parent->append(svb + split_prefix + 1, split_suffix - 1);
        parent_index_len += split_suffix-1;
      }

      bool last_neg = neg[split_dim];
      neg[split_dim] = true;

      value_type min_split_value = min;
      value_type max_split_value = max;
      min_split_value.set(split_value.get(split_dim), split_dim);
      max_split_value.set(split_value.get(split_dim), split_dim);
      parent_splits[split_dim]++;
      int left_bytes =
          build(storage, kdd, nodes, true, leaves_offset, num_left_leaves,
                left_block_fp, from, mid, min, max_split_value, parent_splits,
                leaf_block_fps, last_split_value, neg);
      neg[split_dim] = false;
      int right_bytes =
          build(storage, kdd, nodes, false, right, num_right_leaves,
                left_block_fp, mid, to, min_split_value, max, parent_splits,
                leaf_block_fps, last_split_value, neg);
      // write left bytes
      if (num_left_leaves != 1) {
        parent_index_len += parent->write_vint(left_bytes);
      }
      // write right leaf fp
      if (num_right_leaves == 1) {  
        long left_block_fp = leaf_block_fps[right];
        long delta = left_block_fp - min_block_fp;
        // dump delta
        std::shared_ptr<MemoryFile> parent(new MemoryFile());
        nodes.push_back(parent);
        parent_index_len += parent->write_vint(delta);
      }
      parent_splits[split_dim]--;
      neg[split_dim] = last_neg;
      return left_bytes + right_bytes + parent_index_len;
    }
  }

  void pack(values_type* storage, File* kdm, File* kdi, File* kdd) {
    int from = 0, to = storage->size();
    int count_per_leaf=2;
    int num_leaves = (to - from + 1) / count_per_leaf;
    value_type min, max;
    std::vector<int> parent_splits(value_type::dim, 0);
    std::vector<off_t> leaf_block_fps(num_leaves + 1, 0);
    value_type last_split_value;
    std::vector<bool> neg(value_type::dim, false);
    std::vector<std::shared_ptr<File>> nodes;
    build(storage, kdd, nodes, false, 0, num_leaves, 0, from, to, min, max,
          parent_splits, leaf_block_fps, last_split_value, neg);
    storage->minmax(min_,max_);
    //int ret=kdm->write_vint();
    for(auto& node:nodes){
      std::string content;
      node->read(content,node->size());
      kdi->append(content);
    }
  }

  void intersect(value_type low, value_type high,File* kdi,File* kdd) {
    IntersectState is(3,0,kdi,kdd);
    is._low = low;
    is._high = high;
    do_intersect(min_, max_, is);
    std::cout<<"dump intersect docids:"<<std::endl;
    std::copy(is.docids_.begin(),is.docids_.end(),std::ostream_iterator<int>(std::cout,","));
    std::cout<<std::endl;
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

  void write_docids(File* kdd, std::vector<int>& docids) {
    int count=docids.size();
    if(count==0)
      return;
    kdd->write_vint(count);
    bool sorted=true;
    int pre=docids[0];
    int min=docids[0];
    for(int i=1;i<docids.size();++i){
      if(pre>docids[i]){
        sorted=false;
      }
      if (min>docids[i]){
        min=docids[i];
      }
      pre=docids[i];
    }

    if(sorted){
      char magic=0;
      kdd->append(&magic,1);
      int pre=0;
      for(int i=0;i<docids.size();++i){
        int delta=docids[i]-pre;
        kdd->write_vint(delta);
        pre=docids[i];
      }
    }else{
      char magic=1;
      kdd->append(&magic,1);
      kdd->write_vint(min);
      for(int i=0;i<docids.size();++i){
        int diff=docids[i]-min;
        kdd->write_vint(diff);
      }
    }

  }

  void write_common_prefix(File* kdd, std::vector<int>& common_prefix_lengths,
                           value_type& v) {
    for (int i = 0; i < value_type::dim; ++i) {
      kdd->write_vint(common_prefix_lengths[i]);
      u_char* bytes = v.get_bytes(i);
      kdd->append(bytes, common_prefix_lengths[i]);
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
    storage->minmax(from, to, min, max);

    for (int dim = 0; dim < value_type::dim; ++dim) {
      int common_prefix_length = common_prefix_lengths[dim];
      int suffix_length = value_type::bytes_per_dim - common_prefix_length;
      if (suffix_length > 0) {
        // int index = dim * value_type::bytes_per_dim;
        u_char* min_dim_data = min.get_bytes_one_dim(dim);
        u_char* max_dim_data = max.get_bytes_one_dim(dim);
        kdd->append(min_dim_data + common_prefix_length, suffix_length);
        kdd->append(max_dim_data + common_prefix_length, suffix_length);
      }
    }
  }

  void write_high_cardinality_block_values(
      values_type* storage, File* kdd, int from, int to,
      std::vector<int>& common_prefix_lengths, int sorted_dim) {
    if (value_type::dim != 1) {
      write_minmax(storage, kdd, from, to, common_prefix_lengths);
    }

    int run_len_pos = common_prefix_lengths[sorted_dim];
    // sorted dim has another prefix, ++
    common_prefix_lengths[sorted_dim]++;
    int offset = 0;
    for (int i = from; i < to;) {
      // do run-length compression
      u_char run = run_len(storage, from, from + std::min(to - i, 255),
                           sorted_dim, run_len_pos);
      value_type current = storage->get(i);
      u_char next = current.get_byte(sorted_dim, run_len_pos);
      kdd->append(&next, 1);
      kdd->append(&run, 1);

      for (int j = i; j < i + run; j++) {
        value_type one = storage->get(j);
        for (int k = 0; k < value_type::dim; ++k) {
          u_char* dim_data = one.get_bytes(k);
          kdd->append(dim_data + common_prefix_lengths[k],
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
          kdd->append(dim_data + common_prefix_lengths[j],
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
        /* compute cost of run_len compression
         */
        int num_run_lens = 0;
        for (int i = 0; i < count;) {
          /*do run-length compression
           *we don't need to check the pos,because  we found the sorted_dim wiht
           *least unique, if prefix_length==bytes_per_dim,we should handle it in
           *previous condition
           */
          int pos =
              value_type::bytes_per_dim - common_prefix_lengths[sorted_dim] - 1;
          int run = run_len(storage, from + i, to, sorted_dim, pos);
          ++num_run_lens;
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

  void read_common_prefixes(IntersectState& is,std::vector<int>& common_prefixes,value_type& v){
    long fp=is._left_fp[is.level_];
    for(int i=0;i<value_type::dim;++i){
      int prefix;
      int ret=is.kdd_->read_vint(prefix);
      fp+=ret;
      common_prefixes.push_back(prefix);
      u_char* dim_start=v.get_bytes(i);
      ret=is.kdd_->read(fp,dim_start,prefix);
      fp+=ret;
    }
    is._left_fp[is.level_]=fp;
  }

  void read_minmax(IntersectState& is,std::vector<int>& common_prefixes,value_type& min,value_type& max){
    long fp=is._left_fp[is.level_];
    for (int dim = 0; dim < value_type::dim; dim++) {
      int prefix = common_prefixes[dim];
      u_char* min_dim_start=min.get_bytes(dim);
      u_char* max_dim_start=max.get_bytes(dim);
      int ret=is.kdd_->read(fp, min_dim_start+prefix, value_type::bytes_per_dim - prefix);
      fp+=ret;
      ret=is.kdd_->read(fp,max_dim_start+prefix,value_type::bytes_per_dim - prefix);
      fp+=ret;
    }
    is._left_fp[is.level_]=fp;
  }


  void read_uniq_doc_values(IntersectState& is,int count,value_type& uniq,std::vector<int>& docids){
    if(uniq<is.min_||uniq>is.max_){
      return;
    }
    for(int i=0;i<count;++i){
      is.docids_.push_back(docids[i]);
    }
  }

  void read_low_cardinality_doc_values(IntersectState& is,std::vector<int>& common_prefixes,int count,value_type& incomplete_value,std::vector<int>& docids){
    long fp=is._left_fp[is.level_];
    int i;
    for (i = 0; i < count;) {
      int length=0; 
      int ret=is.kdd_->read_vint(fp,length);
      fp+=ret;
      value_type value=incomplete_value;
      for(int dim = 0; dim < value_type::dim; dim++) {
        int prefix = common_prefixes[dim];
        u_char* dim_start=value.get_bytes(dim);
        ret=is.kdd_->read(fp, dim_start+prefix, value_type::bytes_per_dim - prefix);
        fp+=ret;
      }
      for(int j=0;j<length;++j){
        //is.docvalues_.push_back(value);
        if(value>=is.min_&&value<=is.max_){
          is.docids_.push_back(docids[i]);
        }
      }
    i += length;
    }
   is._left_fp[is.level_]=fp;   
  }

  void read_high_cardinaliy_doc_values( IntersectState& is,std::vector<int>& common_prefixes,int sorted_dim,int count,value_type& incomplete_value,std::vector<int>& docids){
    // the byte at `offset` is compressed using run-length compression,
    // other suffix bytes are stored verbatim
     long fp=is._left_fp[is.level_];
    int offset = common_prefixes[sorted_dim];
    common_prefixes[sorted_dim]++;
    int i;
    for (i = 0; i < count; ) {
      value_type v=incomplete_value;
      char magic;
      int ret=is.kdd_->read(fp,&magic,1);
      fp+=ret;
      u_char* dim_start=v.get_bytes(sorted_dim);
      dim_start[offset]=magic;
      char run_len;
      ret=is.kdd_->read(fp,&run_len,1);
      fp+=ret;
      for (int j = 0; j < run_len; ++j) {
        for(int dim = 0; dim < value_type::dim; dim++) {
          u_char* dim_start=v.get_bytes(dim);
          int prefix = common_prefixes[sorted_dim];
          ret=is.kdd_->read(fp,dim_start,dim_start+prefix, value_type::bytes_per_dim- prefix);
          fp+=ret;
        }
        if(v>=is.min_&&v<=is.max_){
          //is.docvalues_.push_back(v);
          is.docids_.push_back(docids[i]);
        }
      }
      i += run_len;
    }
    is._left_fp[is.level_]=fp;
  }

  

  void read_doc_values(IntersectState& is,std::vector<int>& docids){
    std::vector<int> common_prefixes;
    value_type incomplete_value;
    read_common_prefixes(is,common_prefixes,incomplete_value);
    long fp=is._left_fp[is.level_];
    int count=0;
    int ret=is.kdd_->read_vint(fp,count);
    ret+=fp;
    int sorted_dim=0;
    ret=is.kdd_->read_vint(fp,sorted_dim);
    value_type min=incomplete_value,max=incomplete_value;
    switch (sorted_dim){
      case  -1:{
        read_uniq_doc_values(is,count,incomplete_value,docids);
        break;
      }
      case -2:{
        read_minmax(common_prefixes,min,max);
        read_low_cardinality_doc_values(is,common_prefixes,count,incomplete_value,docids);
        break;
      }
      default:{
        read_minmax(common_prefixes,min,max);
        read_high_cardinaliy_doc_values(is,common_prefixes,sorted_dim,count,incomplete_value,docids);
        break;
      }
    }
  }

  void read_docids(IntersectState& is,std::vector<int>& docids){
    long fp=is._left_fps[is.level_];
    int count=0;
    int ret=is.kdd_->read_vint(fp,count);
    fp+=ret;
    char magic;
    ret=is.kdd_->read(fp,&magic,1);
    fp+=ret;
    switch(magic){
      case 0:{
        int pre=0;
        for(int i=0;i<count;++i){
          int delta=0;
          ret=is.kdd_->read_vint(fp,delta);
          fp+=ret;
          pre+=delta;
          docids.push_back(pre);
        }
        break;
      }
      case 1:{
        int min;
        ret=is.kdd_->read_vint(fp,min);
        fp+=ret;
        for(int i=0;i<count;++i){
          int diff=0;
          ret=is.kdd_->read_vint(fp,diff);
          fp+=ret;
          docids.push_back(min+diff);
        }
        break;
      }
      default:{
        break;
      }
    }
    is._left_fps[is.level_]=fp;
  }

  void add_all(IntersectState& is) {
    if (is.is_leaf()) {
      read_docids(is,is.docids_);
    } else {
      is.push_left();
      add_all(is);
      is.pop();
      is.push_right();
      add_all(is);
      is.pop();
    }
  }

  
  void interfect_one_block(IntersectState& is){
    std::vector<int> docids;
    read_docids(is,docids);
    read_doc_values(is,docids);
  }

  void do_intersect(value_type& min, value_type& max, IntersectState& is) {
    int rel=is.relation(min,max);
    if (rel==0) {
      return;
    } else if (rel==1) {
      add_all(is);
    } else if (is.is_leaf()) {
    } else {
      int split_dim = is._split_dim;
      T split_dim_value = is._split_dim_value;
      value_type split_value = max;
      split_value.set(split_dim_value,split_dim);
      is.push_left();
      do_intersect(min, split_value, is);
      is.pop();
      split_value = min;
      split_value.set(split_dim_value,split_dim);
      is.push_right();
      do_intersect(split_value, max, is);
      is.pop();
    }
  }

  int _depth;
  int _num_leaves;
  value_type min_;
  value_type max_;
};
}  // namespace yas