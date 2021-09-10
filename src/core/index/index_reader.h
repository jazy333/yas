#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/document/field_info.h"
#include "core/index/index_option.h"
#include "core/index/index_stat.h"
#include "core/index/segment_files.h"
#include "core/index/sub_index_reader.h"

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
  void set_option(const IndexOption& option);
  IndexOption& get_option();
  IndexStat get_index_stat();
  std::unordered_map<std::string, FieldInfo> get_field_infos();

 private:
  int get_segement_files(std::vector<SegmentFiles>& files);

 private:
  std::vector<std::shared_ptr<SubIndexReader>> sub_index_readers_;
  IndexOption option_;
  std::unordered_map<std::string, FieldInfo> field_infos_;
  IndexStat index_stat_;
};
}  // namespace yas