#include "serialized_invert_fields_index_reader.h"

#include <gtest/gtest.h>

#include <iostream>

namespace yas {
TEST(SerializedInvertFieldsIndexReader, constructor) {
  std::string invert_index_file = "data/index/segment.1.hdb";
  SerializedInvertFieldsIndexReader invert_index_reader(invert_index_file);
  invert_index_reader.open();
  Term term("百度", "content");
  TermReader* reader = invert_index_reader.get_reader(&term);
  while (reader->next() != NDOCID) {
    //std::cout << "docid:" << reader->docid() << std::endl;
  }
}
}  // namespace yas