#include "boolean_expression.h"

namespace yas {
BooleanExpression::BooleanExpression(Query* query, Operator op)
    : query_(query), op_(op) {}

BooleanExpression::~BooleanExpression() {}

Operator BooleanExpression::get_operator() { return op_; }

Query* BooleanExpression::get_query() { return query_; }

void BooleanExpression::set_query(Query* query) { query_ = query; }

void BooleanExpression::set_operator(Operator op) { op_ = op; }

}  // namespace yas