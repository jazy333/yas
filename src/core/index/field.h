#pragma once
#include <string>

#include "field_index_writer.h"

namespace yas {
class Field {
 public:
  Field(std::string name) : name_(name);
  virtual std::string get_name() { return name_; }
  virtual FieldIndexWriter* get_field_index_writer()=0;

 private:
  std::string name_;
};
}  // namespace yas