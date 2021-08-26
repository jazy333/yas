#pragma once
#include <memory>
#include <string>

namespace yas {
class FieldIndexWriter;
class Field {
 public:
  Field(const std::string& name) : name_(name) {}
  virtual ~Field() = default;
  virtual std::string get_name() { return name_; }
  virtual int get_field_id() { return field_id_; }
  virtual int get_index_type() { return index_type_; }
  virtual bool get_store() { return store_; }
  virtual void set_name(const std::string& name) { name_ = name; }
  virtual void set_field_id(int field_id) { field_id_ = field_id; }
  virtual void set_index_type(int index_type) { index_type_ = index_type; }
  virtual void set_store(bool store) { store_=store; }

  virtual FieldIndexWriter* make_field_index_writer() = 0;

 private:
  std::string name_;
  int field_id_;
  int index_type_;
  bool store_;
};
}  // namespace yas