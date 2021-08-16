#pragma once
#include <cstdint>

namespace yas {
struct FieldIndexMeta {
  int32_t filed_id;
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
  int field_id_;
  int num_dims_;
  int max_count_per_leaf_;
  int bytes_per_dim_;
  int num_leaves_;
  value_type min_;
  value_type max_;
  int count_;
  long data_fp_;
  long index_fp_;
};
}  // namespace yas