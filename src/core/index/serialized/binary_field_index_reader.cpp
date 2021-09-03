#include "binary_field_index_reader.h"

namespace yas {
BinaryFieldIndexReader::BinaryFieldIndexReader(BinaryFieldMeta* meta, File* fvd)
    : meta_(meta) {
  field_values_slice_ = std::unique_ptr<FileSlice>(new FileSlice(
      fvd, meta_->field_values_data_offset, meta_->field_values_data_len));
  if (meta_->max_len != meta_->min_len) {
    field_values_len_slice_ = std::unique_ptr<FileSlice>(new FileSlice(
        fvd, meta_->field_lengths_data_offset, meta_->field_lengths_data_len));
  }
  if (meta_->docids_length != 0)
    posting_lists_ = std::unique_ptr<RoaringPostingList>(new RoaringPostingList(
        fvd, meta_->docids_offset, meta_->docids_length,
        meta_->jump_table_entry_count, meta_->num_values));
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
    field_values_len_slice_->seek(block_index * sizeof(uint64_t));
    uint64_t block_value;
    field_values_len_slice_->read(&block_value, sizeof(block_value));
    return (block_value >> bit_index) & mask;
  } else {
    field_values_len_slice_->seek(block_index*sizeof(uint64_t));
    uint64_t block_values[2];
    field_values_len_slice_->read(block_values, sizeof(block_values));
    int end_bits = 64 - bit_index;
    return ((block_values[0] >> bit_index) | (block_values[1] << end_bits)) &
           mask;
  }
}

void BinaryFieldIndexReader::get(uint32_t docid, std::vector<uint8_t>& value) {
  uint32_t index = 0;
  if (!posting_lists_) {
    index = docid-1;
  } else {
    bool exist = posting_lists_->advance_exact(docid);
    if (exist) {
      index = posting_lists_->index();
    } else {
      value = {};
      return;
    }
  }

  if (meta_->max_len != meta_->min_len) {
    // TODO: get field value length offset
    uint64_t start_offset = get_value(index);
    uint64_t end_offset = get_value(index + 1);
    uint64_t len = end_offset - start_offset;
    std::vector<uint8_t> buffer(len, 0);
    field_values_slice_->seek(start_offset);
    field_values_slice_->read(buffer.data(), len);
    value = buffer;
  } else {
    field_values_slice_->seek(index * meta_->max_len);
    std::vector<uint8_t> buffer(meta_->max_len, 0);
    field_values_slice_->read(buffer.data(), meta_->max_len);
    value = buffer;
  }
}

void BinaryFieldIndexReader::get(uint32_t docid, uint64_t& value) {}
}  // namespace yas