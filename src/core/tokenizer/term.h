#pragma once
#include <string>

namespace yas {
class Term {
 public:
  Term(const std::string& term, const std::string& field);
  Term()=default;
  virtual ~Term();
  virtual std::string get_term();
  virtual std::string get_field();
  virtual void set_term(const std::string& name);
  virtual void set_field(const std::string& field);

 private:
  std::string term_;
  std::string field_;
};
}  // namespace yas