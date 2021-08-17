#pragma once
#include "query.h"

namespace yas {
enum Operator : uint8_t { AND = 0, OR, NOT };

struct OperatorHash {
  size_t operator()(const Operator& op) const{ return static_cast<size_t>(op); }
};

class BooleanExpression {
 public:
  BooleanExpression(Query* query, Operator op);
  virtual ~BooleanExpression();
  Operator get_operator();
  Query* get_query();
  void set_query(Query* query);
  void set_operator(Operator op);

 private:
  Query* query_;
  Operator op_;
};

}  // namespace yas