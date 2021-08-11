#pragma once
#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>

#include "document.h"
#include "invert_fields_index_writer.h"
#include "shared_mutex.h"
#include "sub_index_reader.h"

namespace yas {
class IndexWriter {
 public:
  IndexWriter(/* args */);
  IndexWriter(const IndexOption& index_writer_option);
  virtual ~IndexWriter();
  void add_document(std::unique_ptr<Document> doc);
  void flush();
  std::unique_ptr<SubIndexReader> get_sub_index_reader();

 private:
  std::shared_ptr<InvertFieldsIndexWriter> invert_fields_writer_;
  std::unordered_map<std::string, std::shared_ptr<FieldIndexWriter>>
      point_fields_index_writers_;
  std::unordered_map<std::string, std::shared_ptr<FieldIndexWriter>>
      field_values_index_writers_;
  std::unordered_map<std::string, FieldInfo> field_infos_;
  atomic_uint32_t max_doc_;
  IndexOption option_;
  SharedMutex shared_mutex_;
};

}  // namespace yas