#pragma once
#include <cstdint>
#include <vector>

#include "field_index_reader.h"
#include "field_index_writer.h"

namespace yas {
class BinaryFieldIndexWriter : public FieldIndexWriter,
                               public FieldValueIndexReader {
 public:
  BinaryFieldIndexWriter(/* args */);
  ~BinaryFieldIndexWriter();
  void flush(FieldInfo fi, uint32_t max_doc,Index,const IndexOption& option) override;
  void add(uint32_t docid, Field* field) override;
  void get(uint32_t docid, uint64_t& value) override;
  void get(uint32_t docid, std::vector<char>& value) override;

 private:
  std::vector<std::vector<uint8_t>> values_;
  std::vector<uint32_t> docids_;
  uint32_t max_length_;
  uint32_t min_length_;
};

}  // namespace yas