#pragma once
#include "field_index_writer.h"
namespace yas {
class NumericFieldIndexWriter : public FieldIndexWriter {
 public:
  void flush() override;
  void add(uint32_t docid, Field* field) override;
  FieldIndexReader* get_reader() override;
  private:
  uint32_t last_docid_;

};
}  // namespace yas