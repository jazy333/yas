#include "index_searcher.h"

#include <gtest/gtest.h>

#include <atomic>
#include <unordered_map>
#include <unistd.h>

#include "boolean_query.h"
#include "delimeter_tokenizer.h"
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
  option.current_segment_no = 0;
  option.dir = "data/index/";
  option.segment_prefix = "merge.";
  option.mode=1;
  auto reader=std::make_shared<IndexReader>(option);
  reader->open();
  IndexSearcher searcher(reader);
  sleep(320);
  auto tokenizer = std::unique_ptr<Tokenizer>(new DelimeterTokenizer());
  std::string text = "系统";
  std::string field = "name";
  auto ti = tokenizer->get_term_iterator(text);
  std::vector<std::shared_ptr<BooleanExpression>> expressions;
  IndexStat index_stat = reader->get_index_stat();
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
    if (doc.doc_lens.count(field) == 1)
      std::cout << "doc len:" << doc.doc_lens[field] << std::endl;
    if (doc.match_term_counts.count(field) == 1)
      std::cout << "term matched len:" << doc.match_term_counts[field]
                 << std::endl;

    auto fv_reader = doc.field_value_reader;
    for (int i = 1; i <= 40; ++i) {
      std::cout << "dump info begin===" << std::endl;
      std::cout << "inner id:" << i << std::endl;
      auto id_reader = fv_reader->get_reader("id");
      if (id_reader) {
        std::string id;
        std::vector<uint8_t> value;
        id_reader->get(i, value);
        id.assign(value.begin(), value.end());
        std::cout << "id:" << id << std::endl;
      }

      auto dociid_reader = fv_reader->get_reader("docid");
      if (dociid_reader) {
        std::string id;
        uint64_t value;
        dociid_reader->get(i, value);
        std::cout << "docid:" << (long)value << std::endl;
      }

      auto importance_reader = fv_reader->get_reader("importance_rank");
      if (importance_reader) {
        uint64_t value = -1;
        importance_reader->get(i, value);
        std::cout << "importance rank:" << value << std::endl;
      }

      auto name_int_reader = fv_reader->get_reader("name_int");
      if (name_int_reader) {
        uint64_t value = -1;
        name_int_reader->get(i, value);
        std::cout << "name_int:" << (long)value << std::endl;
      }

      auto name_dl_reader = fv_reader->get_reader("__name_dl");
      if (name_dl_reader) {
        uint64_t value = -1;
        name_dl_reader->get(i, value);
        std::cout << "name doc len:" << (long)value << std::endl;
      }
      std::cout << "dump info end===" << std::endl;
    }
  }
}
}  // namespace yas