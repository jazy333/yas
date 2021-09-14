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
  struct SubIndexReaderCompareWithUpdatetime {
    bool operator()(std::shared_ptr<SubIndexReader> a,
                    std::shared_ptr<SubIndexReader> b) {
      auto segment_info_a = a->get_segment_info();
      auto segment_info_b = b->get_segment_info();

      return segment_info_a.last_update_time < segment_info_b.last_update_time;
    }
  };

  IndexReader(IndexOption option);
  IndexReader();
  virtual ~IndexReader();
  std::vector<std::shared_ptr<SubIndexReader>> get_sub_index_readers();
  void add(std::shared_ptr<SubIndexReader> reader);
  int open();
  int close();
  int reopen();
  void set_option(const IndexOption& option);
  IndexOption& get_option();
  IndexStat get_index_stat();
  std::unordered_map<std::string, FieldInfo> get_field_infos();

 private:
  int get_segment_files(std::vector<SegmentFiles>& files);

 private:
  std::vector<std::shared_ptr<SubIndexReader>> sub_index_readers_;
  IndexOption option_;
  std::unordered_map<std::string, FieldInfo> field_infos_;
  IndexStat index_stat_;
};
}  // namespace yas