#include "term_query.h"

#include "bm25_relevance.h"
#include "term_matcher.h"

namespace yas {
TermQuery::TermQuery(Term term, IndexStat index_stat)
    : term_(term), index_stat_(index_stat) {}

TermQuery::~TermQuery() {}

std::shared_ptr<Query> TermQuery::rewrite() { return nullptr; }

std::shared_ptr<Matcher> TermQuery::matcher(SubIndexReader* sub_reader) {
  auto invert_reader = sub_reader->invert_index_reader();
  if (!invert_reader) return nullptr;
  auto field_values_reader = sub_reader->field_values_reader();
  std::string doc_len_name = "__" + term_.get_field() + "_dl";
  std::shared_ptr<FieldValueIndexReader> field_index_reader = nullptr;
  if (field_values_reader)
    field_index_reader = field_values_reader->get_reader(doc_len_name);
  auto term_reader = invert_reader->get_reader(term_);
  return std::shared_ptr<Matcher>(new TermMatcher(
      term_reader, new BM25Relevance(), index_stat_, field_index_reader));
}
}  // namespace yas