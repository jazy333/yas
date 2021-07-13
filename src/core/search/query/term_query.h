#pragma once

#include "posting_list.h"
#include "query.h"
#include "sub_index_reader.h"
#include "term.h"

namespace yas {
class TermQuery : public Query {
 public:
  TermQuery(Term* term);
  ~TermQuery();
  Query* rewrite(Query* query) override;
  Matcher* matcher(SubIndexReader* sub_reader) override;
  private:
  Term* term_;
};
}  // namespace yas