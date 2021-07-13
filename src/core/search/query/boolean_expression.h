#pragma once
#include "query.h"

namespace yas {
enum Operator { AND, OR, NOT };
class BooleanExpression {
 public:
  BooleanExpression(Query* query,Operator op);
  ~BooleanExpression();
  Operator get_operator();
  Query* get_query();

 private:
  Query* query_;
  Operator op_;
};

}  // namespace yas