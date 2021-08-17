#pragma once
#include <cstdint>
#include <vector>

#include "field_index_meta.h"
#include "field_index_reader.h"
#include "file_slice.h"

namespace yas {
class BinaryFieldIndexReader : public FieldValueIndexReader {
 public:
  BinaryFieldIndexReader(BinaryFieldMeta* meta, File* fvd);
  virtual ~BinaryFieldIndexReader();
  void get(uint32_t docid, uint64_t& value);
  void get(uint32_t docid, std::vector<char>& value);

 private:
  uint64_t get_value(int index);

 private:
  BinaryFieldMeta* meta_;
  File* field_values_data_;
  std::unique_ptr<FileSlice> field_values_slice_;
  std::unique_ptr<FileSlice> field_values_len_slice_;
  std::unique_ptr<RoaringPostingList> posting_lists_;
};

}  // namespace yas