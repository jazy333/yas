#pragma once
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "boolean_expression.h"
#include "query.h"

namespace yas {
class BooleanQuery : public Query {
 public:
  BooleanQuery(std::vector<BooleanExpression*>& expressions);
  virtual ~BooleanQuery();
  Query* rewrite() override;
  std::unique_ptr<Matcher> matcher(SubIndexReader* sub_reader) override;
  int get_mm();
  std::vector<BooleanExpression*> get_expressions();
  bool only_or();
  bool only_and();

 private:
  std::vector<BooleanExpression*> expressions_;
  int mm_;
  std::unordered_map<Operator, std::unordered_set<Query*>, OperatorHash>
      map_expressions_;
};
}  // namespace yas