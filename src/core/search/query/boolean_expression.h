#pragma once
#include <memory>

#include "query.h"

namespace yas {
enum Operator : uint8_t { AND = 0, OR, NOT };

struct OperatorHash {
  size_t operator()(const Operator& op) const {
    return static_cast<size_t>(op);
  }
};

class BooleanExpression {
 public:
  BooleanExpression(std::shared_ptr<Query> query, Operator op);
  virtual ~BooleanExpression();
  Operator get_operator();
 std::shared_ptr<Query> get_query();
  void set_query(std::shared_ptr<Query> query);
  void set_operator(Operator op);

 private:
  std::shared_ptr<Query> query_;
  Operator op_;
};

}  // namespace yas