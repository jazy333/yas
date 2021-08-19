#include "numeric_field_index_writer.h"

#include "bitpacking_compression.h"
#include "common.h"
#include "mmap_file.h"
#include "numeric_field.h"
#include "roaring_posting_list.h"

namespace yas {
void NumericFieldIndexWriter::flush(FieldInfo fi, uint32_t max_doc,
                                    const IndexOption& option) {
  int field_id = fi.get_field_id();

  std::string file_fvm = option.dir + "/" + option.segment_prefix +
                         std::to_string(option.current_segment_no) + ".fvm";
  File* fvm = new MMapFile;
  fvm->open(file_fvm, true);

  std::string file_fvd = option.dir + "/" + option.segment_prefix +
                         std::to_string(option.current_segment_no) + ".fvd";
  File* fvd = new MMapFile;
  fvd->open(file_fvd, true);

  fvm->append(&field_id, 4);  // field id
  uint8_t type = 0;
  fvm->append(&type, 1);         // field type
  if (last_docid_ == max_doc) {  // no need to docids index
    long tmp = -1;
    fvm->append(&tmp, sizeof(tmp));  // docids offset
    tmp = 0;
    fvm->append(&tmp, sizeof(tmp));  // docids length
    int tmp1 = -1;
    fvm->append(&tmp1, sizeof(tmp1));  // docids jump table entry count
  } else {
    size_t size = fvd->size();
    fvm->append(&size, sizeof(size));  // docids offset
    // docids index with roaring doclists
    uint16_t jump_table_entry_count = RoaringPostingList::flush(fvd, docids_);
    size = fvd->size()-size;
    fvm->append(&size, sizeof(size));  // docids length
    fvm->append(&jump_table_entry_count,
                sizeof(uint16_t));  // docids jump table entry count
  }
  size_t num_values = docids_.size();
  fvm->append(&num_values, sizeof(size_t));  // num of values
  uint8_t num_bits = 0;
  if (max_value_ == min_value_) {
    fvm->append(&num_bits, sizeof(uint8_t));     // num bits
    fvm->append(&min_value_, sizeof(uint64_t));  // min value
  } else {
    uint64_t igcd = 0;
    for (int i = 0; i < values_.size(); ++i) {
      // TODO:get gcd
      uint64_t v = values_[i];
      if (igcd == 0) {
        igcd = v;
        continue;
      }

      if (igcd != 1) {
        if (v > std::numeric_limits<uint64_t>::max() / 2) {
          igcd = 1;
        } else if (min_value_ != 0) {
          igcd = gcd(igcd, v - min_value_);
        }
      } else {
        break;
      }
    }

    num_bits = gccbits((max_value_ - min_value_) / igcd);
    fvm->append(&num_bits, sizeof(uint8_t));     // num bits
    fvm->append(&min_value_, sizeof(uint64_t));  // min value
    fvm->append(&igcd, sizeof(uint64_t));        // gcd
    size_t size = fvd->size();
    fvm->append(&size, sizeof(size));  // field value offset;
    size_t field_value_len;
    BitPackingCompression bpc(num_bits);
    size_t out_size = values_.size();
    uint64_t* compress_out = new uint64_t[values_.size()];
    bpc.compress(values_.data(), values_.size(), reinterpret_cast<uint8_t*>(compress_out), out_size);
    field_value_len = out_size << 3;
    fvd->append(compress_out, field_value_len);
    delete[] compress_out;
    fvm->append(&field_value_len, sizeof(size_t));  // field value length;
  }
  fvm->close();
  fvd->close();
  delete fvm;
  delete fvd;
}

void NumericFieldIndexWriter::add(uint32_t docid,
                                  std::shared_ptr<Field> field) {
  NumericField* nf = dynamic_cast<NumericField*>(field.get());
  uint64_t value = nf->get_value();
  docids_.push_back(docid);
  values_.push_back(value);

  if (value > max_value_) {
    max_value_ = value;
  }

  if (value < min_value_) {
    min_value_ = value;
  }
}

void NumericFieldIndexWriter::get(uint32_t docid, uint64_t& value) {
  auto iter = std::lower_bound(docids_.begin(), docids_.end(), docid);
  if (iter != docids_.end()) {
    int index = std::distance(iter , docids_.begin());
    value = values_[index];
  }

  return;
}

void NumericFieldIndexWriter::get(uint32_t docid, std::vector<uint8_t>& value) {}

}  // namespace yas