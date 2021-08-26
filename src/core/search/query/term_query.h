#pragma once

#include "posting_list.h"
#include "query.h"
#include "sub_index_reader.h"
#include "term.h"
#include "index_stat.h"
#include "term_stat.h"

namespace yas {
class TermQuery : public Query {
 public:
  TermQuery(Term* term, IndexStat index_stat);
  virtual ~TermQuery();
  Query* rewrite() override;
  std::unique_ptr<Matcher> matcher(SubIndexReader* sub_reader) override;

 private:
  Term* term_;
  IndexStat index_stat_;
};
}  // namespace yas