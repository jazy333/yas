#include "binary_field_index_writer.h"
#include "bitpacking_compression.h"
#include "common.h"
#include "roaring_posting_list.h"

#include <cstdint>
#include <limits>
#include <vector>



namespace yas {
BinaryFieldIndexWriter::BinaryFieldIndexWriter() {
  min_length_ = std::numeric_limits<uint32_t>::max();
  max_length_ = std::numeric_limits<uint32_t>::min();
}

BinaryFieldIndexWriter::~BinaryFieldIndexWriter() {}

void BinaryFieldIndexWriter::flush(FieldInfo fi, uint32_t max_doc, Index,
                                   const IndexOption& option) {
  std::string file_fvm = option.dir + "/" + option.segment_prefix +
                         std::to_string(option.current_segment_no) + ".fvm";
  File* fvmm = new MMapFile;
  fvm->open(file_fvm);

  std::string file_fvd = option.dir + "/" + option.segment_prefix +
                         std::to_string(option.current_segment_no) + ".fvd";
  File* fvd = new MMapFile;
  fvd->open(file_fvd);

  int field_id = fi.get_field_id();
  fvm.write(&field_id, sizeof(field_id));
  uint8_t type = 1;
  fvm.write(&type, 1);

  size_t num_value = values_.size();

  if (num_value == max_doc) {
    fvm->write(-1L, 8);  // docids offset
    fvm->write(0L, 8);   // docids length
    uint16_t count = -1;
    fvm->write(count, 2);  // docids jump table entry count
  } else {
    loff_t offset = fvd->size();
    fvm->write(&offset, sizeof(offset));  // field data offset
    // docids index with roaring doclists
    uint16_t jump_table_entry_count = RoaringPostingList::make(docids_, fvd);
    size = fvd->size();
    fvm->write(&size, sizeof(size));  // docids length
    fvm->write(
        &jump_table_entry_count,
        sizeof(jump_table_entry_count));  // docids jump table entry count
  }

  fvm->write(&num_value, sizeof(num_value));
  fvm->write(&min_length_, sizeof(min_length_));
  fvm->write(&max_length_, sizeof(max_length_));

  loff_t offset = fvd->size();
  fvm->write(&offset, sizeof(offset));  // value data offset
  std::vector<uint64_t> lens(values_.size() + 1, 0);
  for (int i = 0; i < values_.size(); ++i) {
    fvd->write(values_[i].data(), values_[i].size());
    if (min_length_ != max_length_) {
      lens[i] = lens[i - 1] + values_[i].size();
    }
  }
  offset = fvd->size() - offset;
  fvm->write(&offset, sizeof(offset));  // value data len
  if (min_length_ != max_length_) {
    // write value len index compress
    uint8_t max_bits = gcc_bits(*(lens.rbegin()));
    fvm->write(&max_bits, sizoef(max_bits));
    BitPackingCompression bc(max_bits);
    offset = fvd->size();
    fvm->write(&offset, sizeof(offset));  // value lens data offset
    size_t compress_out_size = lens.size();
    uint64_t* compress_out = new uint64_t[compress_out_size];
    bc.compress(lens.data, lens.size(), compress_out, compress_out_size);
    compress_out_size = compress_out_size << 3;
    fvd->write(compress_out, compress_out_size);
    fvm->write(&compress_out_size, sizeof(compress_out_size));
    delete[] compress_out;
  }
}

void BinaryFieldIndexWriter::add(uint32_t docid, std::shared_ptr<Field> field) {
  std::vector<uint8_t> value = field->get_value();
  if (value.size() > max_length_) {
    max_length_ = value.size();
  }

  if (value.size() < min_length_) {
    min_length_ = value.size();
  }
  values_.emplace_back(value);
  docids_.push_back(docid);
}

std::vector<uint8_t> BinaryFieldIndexWriter::get(uint32_t target) {
  auto iter = std::lower_bound(docids_->begin(), docids_.end(), docid);
  if (iter != docids_.end()) {
    int index = std::distance(iter - docids_->begin());
    return values_[index];
  }

  return {};
}
}  // namespace yas