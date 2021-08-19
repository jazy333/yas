#include "numeric_field_index_reader.h"

namespace yas {
NumericFieldIndexReader::NumericFieldIndexReader(NumericFieldMeta* meta,
                                                 File* fvd)
    : meta_(meta) {
  field_values_slice_ = std::unique_ptr<FileSlice>(new FileSlice(
      fvd, meta_->field_values_data_len, meta_->field_values_data_offset));
  if (meta_->docids_offset != 0)
    posting_lists_ = std::unique_ptr<RoaringPostingList>(new RoaringPostingList(
        fvd, meta_->docids_offset, meta_->docids_offset,
        meta_->jump_table_entry_count, meta_->num_values));
}

uint64_t NumericFieldIndexReader::get_value(uint32_t index) {
  int most_sig_bits = 64 - meta_->num_bits;
  uint64_t mask = ~0L << most_sig_bits;
  mask = mask >> most_sig_bits;

  uint64_t total_bits = index * meta_->num_bits;
  int block_index = total_bits / 64;
  field_values_slice_->seek(block_index);
  int bit_index = total_bits % 64;

  uint64_t value;
  if (bit_index <= most_sig_bits) {  // one block
    uint64_t in;
    field_values_slice_->read(&in, sizeof(in));
    value = ((in >> bit_index) & mask) * meta_->gcd + meta_->min_value;
  } else {  // two blocks
    uint64_t in[2];
    field_values_slice_->read(&in, sizeof(in));
    int end_bits = 64 - bit_index;
    value = ((in[0] >> bit_index) | (in[1] << end_bits)) * meta_->gcd +
            meta_->min_value;
  }
  return value;
}

NumericFieldIndexReader::~NumericFieldIndexReader() {}

void NumericFieldIndexReader::get(uint32_t docid, uint64_t& value) {
  uint32_t index = 0;
  if (posting_lists_) {
    index = docid;
  } else {
    bool exist = posting_lists_->advance_exact(docid);
    if (exist) {
      index = posting_lists_->index();
    } else
      value = -1;
  }

  if (meta_->num_bits == 0) {
    value = meta_->min_value;
  } else {
    value = get_value(index);
  }
}

void NumericFieldIndexReader::get(uint32_t docid, std::vector<uint8_t>& value) {}
}  // namespace yas