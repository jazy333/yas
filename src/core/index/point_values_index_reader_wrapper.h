#pragma once
#include "field_index_reader.h"

namespace yas {
class PointValuesIndexReaderWrapper : public PointValuesIndexReader {
 public:
  PointValuesIndexReaderWrapper(
      std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers);
      virtual ~PointValuesIndexReaderWrapper()=default;
  int open();
  int close();
  PointValueIndexReader* get_reader(const std::string& field_name);
  private:
   std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers_;
};
}  // namespace yas