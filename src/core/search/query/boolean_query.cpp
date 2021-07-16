#include <algorithm>

#include "boolean_query.h"
#include "and_posting_list.h"
#include "none_match_query.h"
#include "not_posting_list.h"
#include "or_posting_list.h"
#include "weak_and_posting_list.h"

namespace yas {
BooleanQuery::BooleanQuery(std::vector<BooleanExpression*> expressions) {}
BooleanQuery::~BooleanQuery() {}

bool BooleanQuery::only_or() {
  return map_expressions_.count(Operator::OR) == 1 &&
         (map_expressions_[Operator::OR].size() == expressions_.size());
}

Query* BooleanQuery::rewrite() {
  // case 1: zero expression,recursive termitate
  if (expressions.size() == 0) {
    return new NoneMatchQuery();
  }

  // case 2: one expression ,recursion termitate
  if (expressions.size() == 1) {
    if (mm_ <= 1) {
      switch (expressions[0]->get_operator()) {
        case AND:
        case OR: {
          return query;
          break;
        }
        case NOT: {
          return new NoneMatchQuery();
          break;
        }
        default:
          break;
      }
    } else {
      return new NoneMatchQuery();
    }
  }

  // case 3 : children need to do rewrite recursively
  {
    bool rewrited = false;
    for (size_t i = 0; i < expressions_.size(); ++i) {
      Query* exp = expressions_[i];
      Query* query = exp->get_query()->rewrite();
      if (query != exp) {
        delete exp;
        expressions[i] = query;
        rewrited = true;
      }
    }
    if (rewrited) {
      return this;
    }
  }

  // case 4: unique all expressions
  {
    size_t uniq_expression_size = 0;

    for (auto&& op : map_expressions_) {
      size += op.size();
    }
    std::vector<BooleanExpression*> expressions;
    if (size != expressions_.size()) {
      for (auto&& op : map_expressions_) {
        for (auto&& exp::op) {
          expressions.push_back(exp);
        }
      }
      return this;
    }
  }

  // case 5: check and ,or with same query
  {
    if (map_expressions.count(Operator::AND) != 0 &&
        map_expressions_.count(Operator::NOT) != 0) {
      std::vector<Query*> intersection;
      std::set_intersection(map_expressions_[Operator::AND].begin(),(map_expressions_[Operator::AND].end(),
      map_expressions_[Operator::NOT].begin(),map_expressions_[Operator::NOT].end(),
      std::back_inserter(intersection));
      if(intersection.size()!=0){
        return new NoneMatchQuery();
      }
    }
  }

  // case 6:remove AnyMatchQuery
  {

  }

  // case 7: flat all boost OR expressions
  {

  }

  // case 8: flat all boost AND expressions
  {

  }

  // case 9： flat all OR expressions
  {
    if (mm_ <= 1) {
      std::vector<BooleanExpression*> expressions;
      bool rewrited = false;
      for (auto&& exp : expressions_) {
        if (exp->get_operator() == Operator::OR &&
            exp->get_query()->only_or()) {
          for (auto&& exp1::exp->get_query()->get_expressions()) {
            expressions.push_back(exp1);
            rewrited = true;
          }
        } else {
          expressions.push_back(exp);
        }
      }
      return new BooleanQuery(expressions);
    }
  }

  return Query::rewrite();
}

std::unique_ptr<Matcher> BooleanQuery::matcher(SubIndexReader* sub_reader) {
  if (only_or()) {
    if (map_expressions_.count(Operator::NOT) != 0) {
      return new NotPostingList();
    } else {
      std::vector<PostingList*> pls;
      for (auto&& exp : map_expressions_[Operator::OR]) {
        pls.push_back(exp->get_query()->posting_list(sub_reader));
      }
      return new OrPostingList(pls);
    }
  }

  if (only_and()) {
    if (map_expressions_.count(Operator::NOT) != 0)
      return new NotPostingList();
    else
      return new AndPostingList();
  }

  if (mm_ > 1) {
    PostingList* req;
    return new WeakAndPostingList();

  } else {
    return new OrPostingList();
  }
}

int BooleanQuery::get_mm() { retrun mm_; }
}  // namespace yas