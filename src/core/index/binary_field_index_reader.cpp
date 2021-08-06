#include "binary_field_index_reader.h"

namespace yas {
BinaryFieldIndexReader::BinaryFieldIndexReader(BinaryFieldMeta* meta,
                                               File* fvd) {
  field_values_slice_ = new FileSlice(fvd, meta_->field_values_data_len,
                                      meta_->field_values_data_offset);
  if (meta_->max_len != meta_->max_len) {
    field_values_len_slice_ = new FileSlice(
        fvd, meta->field_lengths_data_offset, meta_->field_lengths_data_len);
  }
  if (meta->docids_offset != 0)
    posting_lists_ =
        new RoaringPostingList(fvd, meta->docids_offset, meta->docids_offset,
                               meta->jump_table_entry_count, meta->num_values);
}

BinaryFieldIndexReader::~BinaryFieldIndexReader() {}

uint64_t BinaryFieldIndexReader::get_value(int index) {
  int total_bits = index * meta_->field_lengths_num_bits;
  size_t block_index = total_bits / 64;
  size_t bit_index = total_bits % 64;
  int most_sig_bits = 64 - meta_->field_lengths_num_bits;
  uint64_t mask = ~0L << most_sig_bits;
  mask = mask >> most_sig_bits;
  if (bit_index <= most_sig_bits) {
    field_values_len_slice_.seek(block_index);
    uint64_t block_value;
    field_values_len_slice_->read(&block_value, sizeof(block_value));
    return (block_value >> bit_index) & mask;
  } else {
    field_values_len_slice_.seek(block_index);
    uint64_t block_values[2];
    field_values_len_slice_->read(block_values, 2 * sizeof(block_value));
    int end_bits = 64 - bit_index;
    return (block_values[0] >> bit_index) | (block_values[1] << end_bits);
  }
}

std::vector<uint8_t> BinaryFieldIndexReader::get(uint32_t docid) {
  uint32_t index = 0;
  if (posting_lists_) {
    index = docid;
  } else {
    bool exsit = posting_lists_->advance_exact(docid);
    if (exist) {
      index = posting_lists_->index();
    } else
      return {};
  }

  if (meta_->max_len != meta_->min_len) {
    // TODO: get field value length offset
    uint64_t start_offset = get_value(index);
    uint64_t end_offset = get_value(index + 1);
    uint64_t len = end_offset - start_offset;
    std::vector<uint8_t> buffer(len, 0);
    field_values_slice_->seek(start_offset);
    field_values_slice_->read(buffer.data(), len);
    return buffer;
  } else {
    field_values_slice_->seek(index * meta_->max_len);
    std::vector<uint8_t> buffer(meta_->max_len, 0);
    field_values_slice_->read(buffer.data(), meta_->max_len);
    return buffer;
  }
}
}  // namespace yas