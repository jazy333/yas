#pragma once
#include "field_index_reader.h"

namespace yas {
class MemoryPointValuesIndexReaderWrapper : public PointValuesIndexReader {
 public:
  MemoryPointValuesIndexReaderWrapper(
      std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers);
  virtual ~MemoryPointValuesIndexReaderWrapper() = default;
  int open();
  int close();
  PointValueIndexReader* get_reader(
      const std::string& field_name,
      std::unique_ptr<PointFieldIndexReader> init_reader);

 private:
  std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers_;
};
}  // namespace yas