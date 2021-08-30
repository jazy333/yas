#include "index_writer.h"

#include <gtest/gtest.h>

#include <fstream>

#include "numeric_field.h"
#include "point_field.h"
#include "text_field.h"

namespace yas {
TEST(IndexWriter, add) {
  IndexOption option;
  option.current_segment_no = 2;
  option.dir = "data/index";
  option.segment_prefix = "segment.";
  IndexWriter writer(option);
  writer.open();

  std::string file = "data/test_docs.txt.uniq";
  std::ifstream ifs;
  ifs.open(file, std::ios_base::in);
  std::string line;

  int count = 0;
  while (std::getline(ifs, line)) {
    auto doc = std::unique_ptr<Document>(new Document);
    if (*(line.rbegin()) == '\n') line = line.substr(0, line.size() - 1);

    auto text = std::shared_ptr<TextField>(new TextField("content", line));
    text->set_index_type(1);
    doc->add_field(text);

    Point<int, 2> point({count, count * 3}, -1);
    auto pfield = std::shared_ptr<PointField<int, 2>>(
        new PointField<int, 2>("point", point));
    pfield->set_index_type(4);
    doc->add_field(pfield);

    auto nfield =
        std::shared_ptr<NumericField>(new NumericField("source", count % 3));
    nfield->set_index_type(2);
    doc->add_field(nfield);

    auto bfield = std::shared_ptr<TextField>(
        new TextField("binary", std::to_string(count)));
    bfield->set_index_type(3);
    doc->add_field(bfield);
    
    ++count;
    writer.add_document(std::move(doc));
  }

  writer.flush();

  ifs.close();
}
}  // namespace yas