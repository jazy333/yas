#pragma once
#include <vector>
#include <memory>

#include "field.h"

namespace yas {
class Document {
 public:
  Document(/* args */);
  virtual ~Document();
  void add_field(std::shared_ptr<Field> field);
  std::vector<std::shared_ptr<Field>> get_fields();

 private:
  std::vector<std::shared_ptr<Field>> fields_;
};

}  // namespace yas