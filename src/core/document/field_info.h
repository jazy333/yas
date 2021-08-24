#pragma once

namespace yas {
class FieldInfo {
 public:
  FieldInfo() = default;
  FieldInfo(int field_id) : filed_id_(field_id) {}
  int get_field_id() { return filed_id_; }
  void set_field_id(int field_id) { filed_id_ = field_id; }

 private:
  int filed_id_;
};
}  // namespace yas