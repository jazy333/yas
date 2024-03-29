#pragma once
#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>

#include "common/shared_mutex.h"
#include "core/document/document.h"
#include "core/index/index_stat.h"
#include "core/index/serialized/invert_fields_index_writer.h"
#include "core/index/sub_index_reader.h"

namespace yas {
class IndexWriter {
 public:
  IndexWriter(/* args */);
  IndexWriter(const IndexOption& index_writer_option);
  IndexWriter(const IndexOption& index_writer_option,
              std::shared_ptr<Tokenizer> tokenizer);
  virtual ~IndexWriter();
  void add_document(std::unique_ptr<Document> doc);
  int open();
  void flush();
  std::unique_ptr<SubIndexReader> get_sub_index_reader();

 private:
  void process_numeric_field(const std::string& field_name,
                             std::shared_ptr<Field> field);
  int write_segment_info();

 private:
  std::shared_ptr<InvertFieldsIndexWriter> invert_fields_writer_;
  std::shared_ptr<InvertFieldsIndexReader> invert_fields_reader_;

  std::unordered_map<std::string, FieldIndexWriter*>
      point_fields_index_writers_;
  std::unordered_map<std::string, PointFieldIndexReader*>
      point_fields_index_readers_;

  std::unordered_map<std::string, std::shared_ptr<FieldIndexWriter>>
      field_values_index_writers_;
  std::unordered_map<std::string, std::shared_ptr<FieldValueIndexReader>>
      field_values_index_readers_;

  std::unordered_map<std::string, FieldInfo> field_infos_;
  std::atomic<uint32_t> max_doc_;
  std::atomic<uint32_t> max_field_num_;
  IndexOption option_;
  SharedMutex shared_mutex_;
  IndexStat index_stat_;
  std::shared_ptr<Tokenizer> tokenizer_;
};

}  // namespace yas