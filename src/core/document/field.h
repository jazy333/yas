#pragma once
#include <string>
#include <memory>

#include "field_index_writer.h"

namespace yas {
class Field {
 public:
  Field(std::string name) : name_(name) {}

  virtual std::string get_name() { return name_; }

  virtual int get_field_id() { return field_id_; }

  virtual int get_index_type() { return index_type_; }

  virtual bool get_store() { return store_; }

  std::unique_ptr<FieldIndexWriter> make_field_index_writer() = 0;
 private:
  std::string name_;
  int field_id_;
  int index_type_;
  bool store_;
};
}  // namespace yas