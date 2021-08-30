#include "invert_fields_index_writer.h"

#include <gtest/gtest.h>

#include <fstream>

#include "text_field.h"

namespace yas {
TEST(InvertFieldsIndexWriter, constructor) {
  IndexStat index_stat;
  InvertFieldsIndexWriter ifiw(&index_stat);
}

TEST(InvertFieldsIndexWriter, add) {
  IndexStat index_stat;
  InvertFieldsIndexWriter ifiw(&index_stat);

  std::vector<std::string> contents = {"中华人民共和国万岁万岁万万岁",
                                       "阿富汗塔利班", "vscode 唤起命令行",
                                       "加油"};
  uint32_t docid = 1;
  for (auto&& content : contents) {
    auto text = std::shared_ptr<TextField>(new TextField("content", content));
    ifiw.add(docid++, text);
  }
  ifiw.close();
}

TEST(InvertFieldsIndexWriter, flush) {
  IndexStat index_stat;
  InvertFieldsIndexWriter ifiw(&index_stat);

  std::string file = "data/test_docs.txt";
  std::ifstream ifs;
  ifs.open(file, std::ios_base::in);
  std::string line;

  uint32_t docid = 1;
  while (std::getline(ifs, line)) {
    if (*(line.rbegin()) == '\n') line = line.substr(0, line.size() - 1);
    auto text = std::shared_ptr<TextField>(new TextField("content", line));
    ifiw.add(docid++, text);
  }
  FieldInfo dummy;
  IndexOption option;
  option.dir = "data/index/";
  option.segment_prefix = "segment.";
  option.current_segment_no = 1;
  ifiw.flush(dummy, docid, option);
  ifiw.close();
}

}  // namespace yas