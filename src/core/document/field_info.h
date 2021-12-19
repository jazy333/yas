#pragma once
#include <string>

namespace yas {
class FieldInfo {
 public:
  FieldInfo() = default;
  FieldInfo(int field_id) : field_id_(field_id) {}
  int get_field_id() const { return field_id_; }
  void set_field_id(int field_id) { field_id_ = field_id; }
  std::string get_field_name() const { return field_name_; }
  void set_field_name(const std::string& field_name) {
    field_name_ = field_name;
  }
  int get_index_type() const { return index_type_; }
  void set_index_type(int index_type) { index_type_ = index_type; }

 private:
  int field_id_;
  int index_type_;
  std::string field_name_;
};
}  // namespace yas