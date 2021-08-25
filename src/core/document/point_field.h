#pragma once

#include "field.h"
#include "field_index_reader.h"
#include "field_index_writer.h"
#include "point_field_index_writer.h"

namespace yas {
template <class T, int D>
class PointFieldIndexWriter;

template <class T, int D>
class PointField : public Field {
 public:
  PointField(const std::string& name, const Point<T, D>& value)
      : Field(name), value_(value) {}
  PointField() : Field("") {}
  virtual ~PointField() = default;
  Point<T, D> get_value() { return value_; }
  FieldIndexWriter* make_field_index_writer() override {
    return new PointFieldIndexWriter<T, D>;
  }

  PointFieldIndexReader* make_serialized_field_index_reader() {
    return new PointFieldIndexWriter<T, D>();
  }

 private:
  Point<T, D> value_;
};
}  // namespace yas