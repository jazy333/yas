#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "field_info.h"
#include "index_option.h"
#include "sub_index_reader.h"
#include "segment_files.h"

namespace yas {
class IndexReader {
 public:
  IndexReader(IndexOption option);
  IndexReader();
  virtual ~IndexReader();
  std::vector<std::shared_ptr<SubIndexReader>> get_sub_index_readers();
  void add(std::shared_ptr<SubIndexReader> reader);
  int open();
  int close();

 private:
  int scan_dir(std::vector<SegmentFiles>& files);

 private:
  std::vector<std::shared_ptr<SubIndexReader>> sub_index_readers_;
  IndexOption option_;
  std::unordered_map<std::string, FieldInfo> field_infos_;
};
}  // namespace yas