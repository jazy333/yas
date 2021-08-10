#pragma once

#include "field.h"

namespace yas {
template <class T, class D>
class PointField : public Field {
  PointField(const std::string& name, const Point<T, D>& value)
      : Field(name), value_(value) {}
  virtual ~PointField() = default;
  Point<T, D> get_value() { return value_; }
  std::unique_ptr<FieldIndexWriter> make_field_index_writer() override{
      return std::unique_ptr<FieldIndexWriter>(new PointFieldIndexWriter<T,D>);
  }
 private:
  Point<T, D> value_;
};
}  // namespace yas