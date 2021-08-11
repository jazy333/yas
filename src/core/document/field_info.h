#pragma once

namespace yas {
class FieldInfo {
 public:
  FieldInfo();
  int get_field_id();

 private:
  int filed_id_;
};
}  // namespace yas