#pragma once
#include <string>

namespace yas {
class Term {
 private:
  Term(std::string term, std::string field);
  virtual ~Term();
  std::string term_;
  std::string field_;

 public:
  virtual std::string get_term();
  virtual std::string get_field();
  virtual void set_term(const std::string& name);
  virtual void set_field(const std::string& name);
};
}  // namespace yas