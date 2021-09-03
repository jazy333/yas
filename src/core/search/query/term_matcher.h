#pragma once

#include "matcher.h"
#include "relevance.h"
#include "term_reader.h"
#include "term_scorer.h"

namespace yas {
class TermMatcher : public Matcher {
 public:
  TermMatcher(std::shared_ptr<TermReader> term_reader, Relevance* rel,
              IndexStat index_stat,
              std::shared_ptr<FieldValueIndexReader> field_value_reader);
  virtual ~TermMatcher();
  std::shared_ptr<PostingList> posting_list() override;
  std::shared_ptr<Scorer> scorer() override;

 private:
  std::shared_ptr<TermReader> term_reader_;
  Relevance* rel_;
};
}  // namespace yas