#include "index_searcher.h"

#include <gtest/gtest.h>

#include <atomic>
#include <unordered_map>

#include "boolean_query.h"
#include "simple_tokenizer.h"
#include "term_query.h"

namespace yas {
TEST(IndexSearcher, search) {
  std::atomic<uint32_t> count(1);
  std::cout << count << std::endl;
  std::unordered_map<std::string, int> test_map;
  test_map["__content_dl"] = 1;
  std::string test_field_name = "__content_dl";
  if (test_map.count(test_field_name) == 1)
    std::cout << "get a key" << std::endl;
  IndexOption option;
  option.current_segment_no = 3;
  option.dir = "data/index";
  option.segment_prefix = "segment.";
  IndexReader reader(option);
  reader.open();
  IndexSearcher searcher(&reader);
  auto tokenizer = std::unique_ptr<Tokenizer>(new SimpleTokenizer(2));
  std::string text = "搜狗翻译";
  std::string field = "content";
  auto ti = tokenizer->get_term_iterator(text);
  std::vector<std::shared_ptr<BooleanExpression>> expressions;
  IndexStat index_stat = reader.get_index_stat();
  while (ti->next()) {
    Term term = ti->term();
    term.set_field(field);
    auto term_query =
        std::shared_ptr<TermQuery>(new TermQuery(term, index_stat));
    auto expression = std::shared_ptr<BooleanExpression>(
        new BooleanExpression(term_query, Operator::OR));
    expressions.push_back(expression);
  }

  BooleanQuery query(expressions);
  MatchSet result;
  searcher.search(&query, result);
  // EXPECT_EQ(398, result.size());
  MatchedDoc doc;
  while (result.next(doc)) {
    std::cout << "docid:" << doc.docid_ << ",score:" << doc.score_ << std::endl;
  }
}
}  // namespace yas