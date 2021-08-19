#include "binary_field_index_writer.h"

#include <cstdint>
#include <limits>
#include <vector>

#include "bitpacking_compression.h"
#include "common.h"
#include "mmap_file.h"
#include "roaring_posting_list.h"
#include "text_field.h"

namespace yas {
BinaryFieldIndexWriter::BinaryFieldIndexWriter() {
  min_length_ = std::numeric_limits<uint32_t>::max();
  max_length_ = std::numeric_limits<uint32_t>::min();
}

BinaryFieldIndexWriter::~BinaryFieldIndexWriter() {}

void BinaryFieldIndexWriter::flush(FieldInfo fi, uint32_t max_doc,
                                   const IndexOption& option) {
  std::string file_fvm = option.dir + "/" + option.segment_prefix +
                         std::to_string(option.current_segment_no) + ".fvm";
  File* fvm = new MMapFile;
  fvm->open(file_fvm, true);

  std::string file_fvd = option.dir + "/" + option.segment_prefix +
                         std::to_string(option.current_segment_no) + ".fvd";
  File* fvd = new MMapFile;
  fvd->open(file_fvd, true);

  int field_id = fi.get_field_id();
  fvm->append(&field_id, sizeof(field_id));
  uint8_t type = 1;
  fvm->append(&type, 1);

  size_t num_value = values_.size();

  if (num_value == max_doc) {//TODO:check whether index docids
    uint64_t tmp = -1;
    fvm->append(&tmp, sizeof(tmp));  // docids offset
    tmp = 0;
    fvm->append(&tmp, sizeof(tmp));  // docids length
    uint16_t count = -1;
    fvm->append(&count, 2);  // docids jump table entry count
  } else {
    loff_t offset = fvd->size();
    fvm->append(&offset, sizeof(offset));  // field data offset
    // docids index with roaring doclists
    uint16_t jump_table_entry_count = RoaringPostingList::flush(fvd, docids_);
    size_t size = fvd->size();
    fvm->append(&size, sizeof(size));  // docids length
    fvm->append(
        &jump_table_entry_count,
        sizeof(jump_table_entry_count));  // docids jump table entry count
  }

  fvm->append(&num_value, sizeof(num_value));
  fvm->append(&min_length_, sizeof(min_length_));
  fvm->append(&max_length_, sizeof(max_length_));

  loff_t offset = fvd->size();
  fvm->append(&offset, sizeof(offset));  // value data offset
  std::vector<uint64_t> lens(values_.size() + 1, 0);
  for (int i = 0; i < values_.size(); ++i) {
    fvd->append(values_[i].data(), values_[i].size());
    if (min_length_ != max_length_) {
      lens[i] = lens[i - 1] + values_[i].size();
    }
  }
  offset = fvd->size() - offset;
  fvm->append(&offset, sizeof(offset));  // value data len
  
  if (min_length_ != max_length_) {
    // write value len index compress
    uint8_t max_bits = gccbits(*(lens.rbegin()));
    fvm->append(&max_bits, sizeof(max_bits));
    BitPackingCompression bc(max_bits);
    offset = fvd->size();
    fvm->append(&offset, sizeof(offset));  // value lens data offset
    size_t compress_out_size = lens.size();
    uint64_t* compress_out = new uint64_t[compress_out_size];
    compress_out_size = compress_out_size >> 3;
    bc.compress(lens.data(), lens.size(),
                reinterpret_cast<uint8_t*>(compress_out), compress_out_size);
    fvd->append(compress_out, compress_out_size);
    fvm->append(&compress_out_size, sizeof(compress_out_size));
    delete[] compress_out;
  }
}

void BinaryFieldIndexWriter::add(uint32_t docid, std::shared_ptr<Field> field) {
  TextField* bf = dynamic_cast<TextField*>(field.get());
  std::string svalue = bf->get_value();
  std::vector<uint8_t> value(svalue.begin(), svalue.end());
  if (value.size() > max_length_) {
    max_length_ = value.size();
  }

  if (value.size() < min_length_) {
    min_length_ = value.size();
  }
  values_.push_back(value);
  docids_.push_back(docid);
}

void BinaryFieldIndexWriter::get(uint32_t docid, std::vector<uint8_t>& value) {
  auto iter = std::lower_bound(docids_.begin(), docids_.end(), docid);
  if (iter != docids_.end()) {
    int index = std::distance(iter, docids_.begin());
    value = values_[index];
  }

  value = {};
}

void BinaryFieldIndexWriter::get(uint32_t docid, uint64_t& value) {}
}  // namespace yas