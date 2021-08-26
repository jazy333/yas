#pragma once
#include <sys/types.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace yas {
struct FieldIndexMeta {
  int32_t field_id;
  uint8_t type;
  uint64_t docids_offset;
  uint64_t docids_length;
  uint16_t jump_table_entry_count;
};

struct NumericFieldMeta : public FieldIndexMeta {
  uint64_t num_values;
  uint8_t num_bits;
  uint64_t min_value;
  uint64_t gcd;
  uint64_t field_values_data_offset;
  uint64_t field_values_data_len;
};

struct BinaryFieldMeta : public FieldIndexMeta {
  uint64_t num_values;
  uint32_t min_len;
  uint32_t max_len;
  uint64_t field_values_data_offset;
  uint64_t field_values_data_len;
  uint8_t field_lengths_num_bits;
  uint64_t field_lengths_data_offset;
  uint64_t field_lengths_data_len;
};

struct PointFieldMeta {
  PointFieldMeta()
      : field_id_(-1),
        num_dims_(0),
        max_count_per_leaf_(0),
        bytes_per_dim_(0),
        num_leaves_(0),
        min_(nullptr),
        max_(nullptr),
        count_(0),
        data_fp_(0),
        index_fp_(0) {}

  ~PointFieldMeta() {
    if (min_) delete[] min_;
    min_ = nullptr;
    if (max_) delete[] max_;
    max_ = nullptr;
  }

  PointFieldMeta(const PointFieldMeta& other) {
    field_id_ = other.field_id_;
    num_dims_ = other.num_dims_;
    max_count_per_leaf_ = other.max_count_per_leaf_;
    bytes_per_dim_ = other.bytes_per_dim_;
    num_leaves_ = other.num_leaves_;
    min_ = new u_char[bytes_per_dim_ * num_dims_]();
    memcpy(min_, other.min_, bytes_per_dim_ * num_dims_);
    max_ = new u_char[bytes_per_dim_ * num_dims_]();
    memcpy(max_, other.max_, bytes_per_dim_ * num_dims_);

    count_ = other.count_;
    data_fp_ = other.data_fp_;
    index_fp_ = other.index_fp_;
  }

  PointFieldMeta(PointFieldMeta&& other) {
    field_id_ = other.field_id_;
    other.field_id_ = -1;
    num_dims_ = other.num_dims_;
    other.num_dims_ = 0;
    max_count_per_leaf_ = other.max_count_per_leaf_;
    other.max_count_per_leaf_ = 0;
    bytes_per_dim_ = other.bytes_per_dim_;
    other.bytes_per_dim_ = 0;
    num_leaves_ = other.num_leaves_;
    other.num_leaves_ = 0;
    if (min_) delete[] min_;
    min_ = other.min_;
    other.min_ = nullptr;
    if (max_) delete[] max_;
    max_ = other.max_;
    other.max_ = nullptr;
    count_ = other.count_;
    other.count_ = 0;
    data_fp_ = other.data_fp_;
    other.data_fp_ = 0;
    index_fp_ = other.index_fp_;
    other.index_fp_ = 0;
  }

  PointFieldMeta& operator=(const PointFieldMeta& other) {
    if (&other != this) {
      field_id_ = other.field_id_;
      num_dims_ = other.num_dims_;
      max_count_per_leaf_ = other.max_count_per_leaf_;
      bytes_per_dim_ = other.bytes_per_dim_;
      num_leaves_ = other.num_leaves_;
      if (!min_) {
        min_ = new u_char[bytes_per_dim_ * num_dims_]();
      }
      memcpy(min_, other.min_, bytes_per_dim_ * num_dims_);

      if (!max_) {
        max_ = new u_char[bytes_per_dim_ * num_dims_]();
      }
      memcpy(max_, other.max_, bytes_per_dim_ * num_dims_);

      count_ = other.count_;
      data_fp_ = other.data_fp_;
      index_fp_ = other.index_fp_;
    }
    return *this;
  }

  PointFieldMeta& operator=(PointFieldMeta&& other) {
    if (&other != this) {
      field_id_ = other.field_id_;
      other.field_id_ = -1;
      num_dims_ = other.num_dims_;
      other.num_dims_ = 0;
      max_count_per_leaf_ = other.max_count_per_leaf_;
      other.max_count_per_leaf_ = 0;
      bytes_per_dim_ = other.bytes_per_dim_;
      other.bytes_per_dim_ = 0;
      num_leaves_ = other.num_leaves_;
      other.num_leaves_ = 0;
      min_ = other.min_;
      other.min_ = nullptr;
      max_ = other.max_;
      other.max_ = nullptr;
      count_ = other.count_;
      other.count_ = 0;
      data_fp_ = other.data_fp_;
      other.data_fp_ = 0;
      index_fp_ = other.index_fp_;
      other.index_fp_ = 0;
    }

    return *this;
  }

  int field_id_;
  int num_dims_;
  int max_count_per_leaf_;
  int bytes_per_dim_;
  int num_leaves_;
  u_char* min_;
  u_char* max_;
  int count_;
  long data_fp_;
  long index_fp_;
};
}  // namespace yas