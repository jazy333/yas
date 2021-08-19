#pragma once
#include "field_index_reader.h"
#include "field_index_writer.h"
#include "field_info.h"
#include "file.h"

namespace yas {
class NumericFieldIndexWriter : public FieldIndexWriter,
                                public FieldValueIndexReader {
 public:
  NumericFieldIndexWriter() = default;
  virtual ~NumericFieldIndexWriter() = default;
  void flush(FieldInfo fi, uint32_t max_doc,
             const IndexOption& option) override;
  void add(uint32_t docid, std::shared_ptr<Field> field) override;
  void get(uint32_t docid, uint64_t& value) override;
  void get(uint32_t docid, std::vector<uint8_t>& value) override;

 private:
  uint32_t last_docid_;
  std::vector<uint32_t> docids_;
  std::vector<uint64_t> values_;
  uint64_t max_value_;
  uint64_t min_value_;
};
}  // namespace yas