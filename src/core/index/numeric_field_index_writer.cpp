#include "numeric_field_index_writer.h"
#include "bitpacking_compression.h"
#include "numeric_field.h"
#include "roaring_posting_list.h"

namespace yas {
void NumericFieldIndexWriter::flush(FieldInfo fi, uint32_t max_doc, Index,
                                    const IndexOption& option) {
  int field_id = fi.get_field_id();

  std::string file_fvm = option.dir + "/" + option.segment_prefix +
                         std::to_string(option.current_segment_no) + ".fvm";
  File* fvmm = new MMapFile;
  fvm->open(file_fvm);

  std::string file_fvd = option.dir + "/" + option.segment_prefix +
                         std::to_string(option.current_segment_no) + ".fvd";
  File* fvmd = new MMapFile;
  fvm->open(file_fvd);

  fvm->write(&field_id, 4);  // field id
  uint8_t type = 0;
  fvm->write(&type, 1);          // field type
  if (last_docid_ == max_doc) {  // no need to docids index
    fvm->write(-1L, 8);          // docids offset
    fvm->write(0L, 8);           // docids length
    fvm->write(-1, 4);           // docids jump table entry count
  } else {
    size_t size = fvd->size();
    fvm->write(&size, sizeof(size_t));  // docids offset
    // docids index with roaring doclists
    uint16_t jump_table_entry_count = RoaringPostingList::make(docids_, fvd);
    size = fvd->size();
    fvm->write(&size, sizeof(size_t));  // docids length
    fvm->write(&jump_table_entry_count,
               sizeof(uint16_t));  // docids jump table entry count
  }
  size_t num_values = docids_.size();
  fvm->write(&num_values, sizeof(size_t));  // num of values
  uint8_t num_bits = 0;
  if (max_value_ == min_value) {
    fvm->write(&num_bits, sizeof(uint8_t));     // num bits
    fvm->write(&min_value_, sizeof(uint64_t));  // min value
  } else {
    uint64_t gcd = 0;
    for (int i = 0; i < values_.size(); ++i) {
      // TODO:get gcd
      uint64_t v = values_[i];
      if (gcd == 0) {
        gcd = v;
        continue;
      }

      if (gcd != 1) {
        if (v > std::numeric_limits<uint64_t>::max() / 2) {
          gcd = 1;
        } else if (min_value_ != 0) {
          gcd = gcd(gcd, v - min_value_);
        }
      } else {
        break;
      }
    }

    num_bits = gcc_bits((max_value_ - min_value_) / gcd);
    fvm->write(&num_bits, sizeof(uint8_t));     // num bits
    fvm->write(&min_value_, sizeof(uint64_t));  // min value
    fvm->write(&gcd, sizeof(uint64_t));         // gcd
    size = fvd->size();
    fvm->write(&size, sizeof(size_t));  // field value offset;
    size_t field_value_len;
    BitPackingCompression bpc(num_bits);
    size_t out_size = values_.size();
    uint64_t* compress_out = new uint64_t[values_.size()];
    bpc.compress(values_.data(), values_.size(), compress_out, out_size);
    field_value_len = compress_out_size << 3;
    fvd->write(compress_out, field_value_len);
    delete[] compress_out;
    fvm->write(&field_value_len, sizeof(size_t));  // field value length;
  }
  fvm->close();
  fvd->close();
  delete fvm;
  delete fvd;
}

void NumericFieldIndexWriter::add(uint32_t docid, Field* field) {
  NumericField* nf = dynamic_cast<NumericField*>(field);
  uint64_t value = nf->get_value();
  docids_.push_back(docid);
  values_.push_back(value);

  if (value > max_value_) {
    max_ = value;
  }

  if (value < min_value_) {
    min_ = value;
  }
}

void NumericFieldIndexWriter::get(uint32_t docid, uint64_t& value) {
  auto iter = std::lower_bound(docids_->begin(), docids_.end(), docid);
  if (iter != docids_.end()) {
    int index = std::distance(iter - docids_->begin());
    value = values_[index];
  }

  return;
}

void NumericFieldIndexWriter::get(uint32_t docid, std::vector<char>& value) {}
}
}  // namespace yas