#pragma once

#include "field.h"
#include "numeric_field_index_writer.h"

namespace yas {
class NumericFieldIndexWriter;
class NumericField : public Field {
 public:
  NumericField(const std::string& name, const long& value)
      : Field(name), value_(value) {}
  virtual ~NumericField() = default;
  long get_value() { return value_; }
  FieldIndexWriter* make_field_index_writer() override {
    return new NumericFieldIndexWriter();
  }

 private:
  long value_;
};
}  // namespace yas