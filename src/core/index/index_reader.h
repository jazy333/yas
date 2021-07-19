#pragma once
#include <vector>

#include "sub_index_reader.h"

namespace yas {
class IndexReader {
 public:
  std::vector<SubIndexReader*> get_sub_index_reader();

 private:
  std::vector<SubIndexReader*> sub_index_readers_;
};
}  // namespace yas