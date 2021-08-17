#pragma once
#include "field_index_reader.h"
#include <unordered_map>

namespace yas {
class MemoryPointValuesIndexReaderWrapper : public PointFieldsIndexReader {
 public:
  MemoryPointValuesIndexReaderWrapper(
      std::unordered_map<std::string, std::shared_ptr<PointFieldIndexReader>>*
          field_index_readers);
  virtual ~MemoryPointValuesIndexReaderWrapper() = default;
  int open() override;
  int close() override;
  std::shared_ptr<PointFieldIndexReader> get_reader(
      const std::string& field_name,
      std::shared_ptr<PointFieldIndexReader> init_reader) override;

 private:
  std::unordered_map<std::string, std::shared_ptr<PointFieldIndexReader>>*
      field_index_readers_;
};
}  // namespace yas