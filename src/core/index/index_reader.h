#pragma once
#include <memory>
#include <string>
#include <vector>

#include "index_option.h"
#include "sub_index_reader.h"

namespace yas {
class IndexReader {
 public:
  IndexReader(IndexOption option);
  IndexReader();
  virtual ~IndexReader();
  std::vector<std::unique_ptr<SubIndexReader>> get_sub_index_reader();
  void add(std::unique_ptr<SubIndexReader> reader);
  int open();
  int close();

 private:
  int scan_dir(std::vector<segment_files>& files);

 private:
  std::vector<std::unique_ptr<SubIndexReader>> sub_index_readers_;
  IndexOption option_;
}  // namespace yas