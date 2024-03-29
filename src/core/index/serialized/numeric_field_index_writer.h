#pragma once
#include "core/index/field_index_reader.h"
#include "core/index/field_index_writer.h"
#include "core/document/field_info.h"
#include "core/db/file.h"

namespace yas {
class NumericFieldIndexWriter : public FieldIndexWriter,
                                public FieldValueIndexReader {
 public:
  NumericFieldIndexWriter() ;
  virtual ~NumericFieldIndexWriter() = default;
  void flush(FieldInfo fi, uint32_t max_doc,
             const IndexOption& option) override;
  int add(uint32_t docid, std::shared_ptr<Field> field) override;
  void get(uint32_t docid, uint64_t& value) override;
  void get(uint32_t docid, std::vector<uint8_t>& value) override;

 private:
  std::vector<uint32_t> docids_;
  std::vector<uint64_t> values_;
  uint64_t max_value_;
  uint64_t min_value_;
  bool first_;
};
}  // namespace yas