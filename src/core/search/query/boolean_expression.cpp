#include "boolean_expression.h"

namespace yas {
BooleanExpression::BooleanExpression(std::shared_ptr<Query> query, Operator op)
    : query_(query), op_(op) {}

BooleanExpression::~BooleanExpression() {}

Operator BooleanExpression::get_operator() { return op_; }

std::shared_ptr<Query> BooleanExpression::get_query() { return query_; }

void BooleanExpression::set_query(std::shared_ptr<Query> query) {
  query_ = query;
}

void BooleanExpression::set_operator(Operator op) { op_ = op; }

}  // namespace yas