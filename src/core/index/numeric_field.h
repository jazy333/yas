#pragma once

#include "field.h"

namespace yas {

class NumericField : public Field {
  PointField(const std::string& name, const long& value)
      : Field(name), value_(value) {}
  virtual ~PointField() = default;
  long get_value() { return value_; }

 private:
  long value_;
};
}  // namespace yas