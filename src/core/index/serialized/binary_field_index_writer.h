#pragma once
#include <cstdint>
#include <vector>

#include "field_index_reader.h"
#include "field_index_writer.h"

namespace yas {
class BinaryFieldIndexWriter : public FieldIndexWriter,
                               public FieldValueIndexReader {
 public:
  BinaryFieldIndexWriter();
  virtual ~BinaryFieldIndexWriter();
  void flush(FieldInfo fi, uint32_t max_doc,const IndexOption& option) override;
  void add(uint32_t docid, std::shared_ptr<Field>  field) override;
  void get(uint32_t docid, uint64_t& value) override;
  void get(uint32_t docid, std::vector<uint8_t>& value) override;

 private:
  std::vector<std::vector<uint8_t>> values_;
  std::vector<uint32_t> docids_;
  uint32_t max_length_;
  uint32_t min_length_;
};

}  // namespace yas