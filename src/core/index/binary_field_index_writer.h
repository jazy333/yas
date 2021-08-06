#pragma once
#include <cstdint>
#include <vector>

#include "field_index_writer.h"

namespace yas {
class BinaryFieldIndexWriter : public FieldIndexWriter {
 public:
  BinaryFieldIndexWriter(/* args */);
  ~BinaryFieldIndexWriter();
  void flush(File* fvm, File* fvd, FieldInfo fi, uint32_t max_doc) override;
  void add(uint32_t docid, Field* field) override;
  FieldIndexReader* get_reader() override;

 private:
  std::vector<std::vector<uint8_t>> values_;
  std::vector<uint32_t> docids_;
  uint32_t max_length_;
  uint32_t min_length_;
};

}  // namespace yas