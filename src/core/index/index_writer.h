#pragma once
#include "document.h"
#include "field_index_writer.h"
#include "sub_index_reader.h"

#include <unordered_map>
#include <string>

namespace yas {
class IndexWriter {
 public:
  IndexWriter(/* args */);
  ~IndexWriter();
  void add_document(Document* doc);
  void flush();
  SubIndexReader* get_sub_index_readers();

 private:
  std::unordered_map<std::string,FieldIndexWriter*> field_index_writers_;
};

}  // namespace yas