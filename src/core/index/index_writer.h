#pragma once
#include "document.h"
#include "field_index_writer.h"

#include <unordered_map>
#include <string>

namespace yas {
class IndexWriter {
 public:
  IndexWriter(/* args */);
  ~IndexWriter();
  void add_document(Document* doc);
  void flush();

 private:
  std::unordered_map<std::string,FieldIndexWriter*> field_index_writers_;
};

}  // namespace yas