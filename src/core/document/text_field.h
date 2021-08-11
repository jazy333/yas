#include <string>

#include "field.h"

namespace yas {
class TextField : public Field {
 public:
  TextField(const std::string& name, const std::string& value);
  virtual ~TextField() = default;
  std::string get_value();
  std::unique_ptr<FieldIndexWriter> make_field_index_writer() override;

 private:
  std::string value_;
};
}  // namespace yas