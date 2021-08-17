#pragma once
#include <unordered_map>

#include "field_index_reader.h"
#include "field_index_writer.h"

namespace yas {
class MemoryFieldValuesIndexReaderWrapper : public FieldValuesIndexReader {
 public:
  MemoryFieldValuesIndexReaderWrapper(
      std::unordered_map<std::string, std::shared_ptr<FieldValueIndexReader>>*
          field_index_readers);
  int open();
  int close();
  std::shared_ptr<FieldValueIndexReader> get_reader(
      const std::string& field_name);

 private:
  std::unordered_map<std::string, std::shared_ptr<FieldValueIndexReader>>*
      field_index_readers_;
};
}  // namespace yas