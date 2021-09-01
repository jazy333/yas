#pragma once
#include <unordered_map>

#include "field_index_reader.h"

namespace yas {
class MemoryPointValuesIndexReaderWrapper : public PointFieldsIndexReader {
 public:
  MemoryPointValuesIndexReaderWrapper(
      std::unordered_map<std::string, PointFieldIndexReader*>*
          field_index_readers);
  virtual ~MemoryPointValuesIndexReaderWrapper() = default;
  int open() override;
  int close() override;
  PointFieldIndexReader* get_reader(
      const std::string& field_name,
      PointFieldIndexReader* init_reader) override;
  PointFieldMeta* get_meta(int field_id) override;

 private:
  std::unordered_map<std::string, PointFieldIndexReader*>* field_index_readers_;
};
}  // namespace yas