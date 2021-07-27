#pragma once
#include "field.h"

#include <vector>

namespace yas {
class Document {
 public:
  Document(/* args */);
  ~Document();
  void add_field(Field* field);
  std::vector<Field*> get_fields();
 private:
  std::vector<Field*> fields_;
};

}  // namespace yas