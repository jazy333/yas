#pragma once
#include "field_index_reader.h"

namespace yas {
class MemoryFieldValuesIndexReaderWrapper : public FieldValuesIndexReader {
 public:
  MemoryFieldValuesIndexReaderWrapper(
      std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers);
  int open();
  int close();
  FieldValueIndexReader* get_reader(const std::string& field_name);
  private:
   std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers_;
};
}  // namespace yas