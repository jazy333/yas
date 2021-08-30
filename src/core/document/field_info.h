#pragma once
#include <string>

namespace yas {
class FieldInfo {
 public:
  FieldInfo() = default;
  FieldInfo(int field_id) : filed_id_(field_id) {}
  int get_field_id() const { return filed_id_; }
  void set_field_id(int field_id) { filed_id_ = field_id; }
  std::string get_field_name() const { return field_name_; }
  void set_field_name(const std::string& field_name) {
    field_name_ = field_name;
  }

 private:
  int filed_id_;
  std::string field_name_;
};
}  // namespace yas