#pragma once
#include "field_index_meta.h"
#include "field_index_reader.h"
#include "file.h"
#include "roaring_posting_list.h"
#include "file_slice.h"
#include "roaring_posting_list.h"

#include <memory>

namespace yas {
class NumericFieldIndexReader : public FieldValueIndexReader {
 public:
  NumericFieldIndexReader(NumericFieldMeta* meta, File* fvd);
  virtual ~NumericFieldIndexReader();
  uint64_t get(uint32_t docid);
  private:
  uint64_t get_value(uint32_t index);

 private:
  NumericFieldMeta* meta_;
  File* field_values_data_;
  std::unique_ptr<FileSlice> field_values_slice_;
  std::unique_ptr<RoaringPostingList> posting_lists_;
};
}  // namespace yas