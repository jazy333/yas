#pragma once

namespace yas {
class FieldInfo {
 public:
  FieldInfo()=default;
  int get_field_id() { return filed_id_; }

 private:
  int filed_id_;
};
}  // namespace yas