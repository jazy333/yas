#pragma once
#include "field_index_writer.h"
#include "field_info.h"
#include "file.h"


namespace yas {
class NumericFieldIndexWriter : public FieldIndexWriter {
 public:
  void flush() override;
  void add(uint32_t docid, Field* field) override;
  FieldIndexReader* get_reader() override;

 private:
  uint32_t last_docid_;
  std::vector<uint32_t> docids_;
  std::vector<uint64_t> values_;
  uint64_t max_value_;
  uint64_t min_value_;
};
}  // namespace yas