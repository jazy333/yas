#include "numeric_field_index_reader.h"

namespace yas {
NumericFieldIndexReader::NumericFieldIndexReader(NumericFieldMeta* meta,
                                                 File* fvd)
    : meta_(meta), field_values_data_(fvd) {
  field_values_slice_ = new FileSlice(fvd, meta_->field_values_data_len,
                                      meta_->field_values_data_offset);
  if (meta->docids_offset != 0)
    posting_lists_ =
        new RoaringPostingList(fvd, meta->docids_offset, meta->docids_offset,
                               meta->jump_table_entry_count, meta->num_values);
}

uint64_t NumericFieldIndexReader::get_value(uint32_t index) {
  uint64_t* in = int most_sig_bits = 64 - meta->num_bits_;
  uint64_t mask = ~0L << most_sig_bits;
  mask = mask >> most_sig_bits;

  uint64_t total_bits = index * meta->num_bits_;
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
    value = (in[0] >> bit_index  |
            (in[1] <<end_bits) * meta_->gcd + meta_->min_value;
  }
  return value;
}

NumericFieldIndexReader::~NumericFieldIndexReader() {}

uint64_t NumericFieldIndexReader::get(uint32_t docid) {
  uint32_t index = 0;
  if (posting_lists_) {
    index = docid;
  } else {
    bool exsit = posting_lists_->advance_exact(docid);
    if (exist) {
      index = posting_lists_->index();
    } else
      return -1;
  }

  if (meta_->num_bits == 0) {
    return meta_->min_value;
  } else {
    return get_value(index);
  }
}
}  // namespace yas