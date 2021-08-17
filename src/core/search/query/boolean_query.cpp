#include "boolean_query.h"

#include <algorithm>
#include <memory>

#include "and_posting_list.h"
#include "aug_and_scorer.h"
#include "default_matcher.h"
#include "none_match_query.h"
#include "not_posting_list.h"
#include "or_posting_list.h"
#include "weak_and_posting_list.h"
namespace yas {
BooleanQuery::BooleanQuery(std::vector<BooleanExpression*>& expressions) {}

BooleanQuery::~BooleanQuery() {}

bool BooleanQuery::only_or() {
  return map_expressions_.count(Operator::OR) == 1 &&
         (map_expressions_[Operator::OR].size() == expressions_.size());
}

bool BooleanQuery::only_and() {
  return map_expressions_.count(Operator::AND) == 1 &&
         (map_expressions_[Operator::AND].size() == expressions_.size());
}

Query* BooleanQuery::rewrite() {
  // case 1: zero expression,recursive termitate
  if (expressions_.size() == 0) {
    return new NoneMatchQuery();
  }

  // case 2: one expression ,recursion termitate
  if (expressions_.size() == 1) {
    if (mm_ <= 1) {
      switch (expressions_[0]->get_operator()) {
        case AND:
        case OR: {
          return this;
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
      Query* original_query = expressions_[i]->get_query();
      Query* query = original_query->rewrite();
      if (query != original_query) {
        delete original_query;
        expressions_[i]->set_query(query);
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

    for (auto&& item : map_expressions_) {
      uniq_expression_size += item.second.size();
    }
#if 0
    std::vector<BooleanExpression*> expressions;
    if (uniq_expression_size != expressions_.size()) {
      for (auto&& op : map_expressions_) {
        for (auto&& exp : op.second) {
          expressions_.push_back(exp);
        }
      }
      return this;
    }
#endif
  }

  // case 5: check and ,or with same query
  {
    if (map_expressions_.count(Operator::AND) != 0 &&
        map_expressions_.count(Operator::NOT) != 0) {
      std::vector<Query*> intersection;
      std::set_intersection(map_expressions_[Operator::AND].begin(),
                            map_expressions_[Operator::AND].end(),
                            map_expressions_[Operator::NOT].begin(),
                            map_expressions_[Operator::NOT].end(),
                            std::back_inserter(intersection));
      if (intersection.size() != 0) {
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
#if 0
 if (mm_ <= 1) {
      std::vector<BooleanExpression*> expressions;
      bool rewrited = false;
      for (auto&& exp : expressions_) {
        if (exp->get_operator() == Operator::OR &&
            exp->get_query()->only_or()) {
          for (auto&& exp1 : exp->get_query()->get_expressions()) {
            expressions.push_back(exp1);
            rewrited = true;
          }
        } else {
          expressions.push_back(exp);
        }
      }
      return new BooleanQuery(expressions);
    }
#endif
  }
  return Query::rewrite();
}

std::unique_ptr<Matcher> BooleanQuery::matcher(SubIndexReader* sub_reader) {
  if (only_or()) {
    if (map_expressions_.count(Operator::NOT) != 0) {
      std::vector<PostingList*> or_pls;
      for (auto&& query : map_expressions_[Operator::OR]) {
        or_pls.push_back(query->matcher(sub_reader)->posting_list());
      }
      std::vector<PostingList*> not_pls;
      for (auto&& query : map_expressions_[Operator::NOT]) {
        not_pls.push_back(query->matcher(sub_reader)->posting_list());
      }
      PostingList* opt_pl = new OrPostingList(or_pls);
      PostingList* not_pl = new OrPostingList(not_pls);
      PostingList* opt_not = new NotPostingList(opt_pl, not_pl);

      return std::unique_ptr<DefaultMatcher>(
          new DefaultMatcher(opt_not, opt_not));
    } else {
      std::vector<PostingList*> pls;
      for (auto&& query : map_expressions_[Operator::OR]) {
        pls.push_back(query->matcher(sub_reader)->posting_list());
      }
      PostingList* pl = new OrPostingList(pls);
      return std::unique_ptr<DefaultMatcher>(new DefaultMatcher(pl, pl));
    }
  }

  if (only_and()) {
    if (map_expressions_.count(Operator::NOT) != 0) {
      std::vector<PostingList*> and_pls;
      for (auto&& query : map_expressions_[Operator::AND]) {
        and_pls.push_back(query->matcher(sub_reader)->posting_list());
      }
      std::vector<PostingList*> not_pls;
      for (auto&& query : map_expressions_[Operator::NOT]) {
        not_pls.push_back(query->matcher(sub_reader)->posting_list());
      }
      PostingList* and_pl = new AndPostingList(and_pls);
      PostingList* not_pl = new OrPostingList(not_pls);
      PostingList* and_not = new NotPostingList(and_pl, not_pl);

      return std::unique_ptr<DefaultMatcher>(
          new DefaultMatcher(and_not, and_not));
    } else {
      std::vector<PostingList*> pls;
      for (auto&& query : map_expressions_[Operator::AND]) {
        pls.push_back(query->matcher(sub_reader)->posting_list());
      }
      PostingList* pl = new AndPostingList(pls);
      return std::unique_ptr<DefaultMatcher>(new DefaultMatcher(pl, pl));
    }
  }

  if (mm_ > 1) {
    std::vector<PostingList*> or_pls;
    for (auto&& query : map_expressions_[Operator::OR]) {
      or_pls.push_back(query->matcher(sub_reader)->posting_list());
    }

    PostingList* weak_and_pl = new WeakAndPostingList(or_pls, mm_);
    if (map_expressions_.count(Operator::NOT) != 0) {
      std::vector<PostingList*> not_pls;
      for (auto&& query : map_expressions_[Operator::NOT]) {
        not_pls.push_back(query->matcher(sub_reader)->posting_list());
      }
      PostingList* or_pl = new OrPostingList(not_pls);
      PostingList* not_pl = new NotPostingList(weak_and_pl, or_pl);

      return std::unique_ptr<DefaultMatcher>(
          new DefaultMatcher(not_pl, not_pl));
    } else {
      return std::unique_ptr<DefaultMatcher>(
          new DefaultMatcher(weak_and_pl, weak_and_pl));
    }
  } else {
    std::vector<PostingList*> or_pls;
    for (auto&& query : map_expressions_[Operator::OR]) {
      or_pls.push_back(query->matcher(sub_reader)->posting_list());
    }

    std::vector<PostingList*> and_pls;
    for (auto&& query : map_expressions_[Operator::AND]) {
      and_pls.push_back(query->matcher(sub_reader)->posting_list());
    }
    PostingList* or_pl = new OrPostingList(or_pls);
    PostingList* and_pl = new AndPostingList(and_pls);
    Scorer* aug_and_scorer = new AugAndScorer(and_pl, or_pl);
    return std::unique_ptr<DefaultMatcher>(
        new DefaultMatcher(and_pl, aug_and_scorer));
  }
}

int BooleanQuery::get_mm() { return mm_; }

std::vector<BooleanExpression*> BooleanQuery::get_expressions() {
  return expressions_;
  return expressions_;
}

}  // namespace yas