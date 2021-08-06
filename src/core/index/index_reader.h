#pragma once
#include <vector>
#include <string>

#include "sub_index_reader.h"

namespace yas {
class IndexReader {
 public:
  IndexReader(const std::string& dir);
  virtual ~IndexReader();
  std::vector<SubIndexReader*> get_sub_index_reader();
  int open();
  void close();

 private:
  std::vector<SubIndexReader*> sub_index_readers_;
  std::string dir_;
};
}  // namespace yas