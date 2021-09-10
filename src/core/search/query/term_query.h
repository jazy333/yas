#pragma once

#include "core/index/index_stat.h"
#include "core/index/sub_index_reader.h"
#include "core/search/match/posting_list.h"
#include "core/search/query/query.h"
#include "core/search/term_stat.h"
#include "core/tokenizer/term.h"

namespace yas {
class TermQuery : public Query {
 public:
  TermQuery(Term term, IndexStat index_stat);
  virtual ~TermQuery();
  std::shared_ptr<Query> rewrite() override;
  std::shared_ptr<Matcher> matcher(SubIndexReader* sub_reader) override;

 private:
  Term term_;
  IndexStat index_stat_;
};
}  // namespace yas