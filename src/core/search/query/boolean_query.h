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
  BooleanQuery(std::vector<std::shared_ptr<BooleanExpression>>& expressions);
  virtual ~BooleanQuery();
  std::shared_ptr<Query> rewrite() override;
  std::shared_ptr<Matcher> matcher(SubIndexReader* sub_reader) override;
  int get_mm();
  std::vector<std::shared_ptr<BooleanExpression>> get_expressions();
  bool only_or();
  bool only_and();

 private:
  std::vector<std::shared_ptr<BooleanExpression>> expressions_;
  int mm_;
  std::unordered_map<Operator, std::unordered_set<std::shared_ptr<Query>>,
                     OperatorHash>
      map_expressions_;
};
}  // namespace yas