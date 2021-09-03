#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "field_index_reader.h"
#include "field_index_writer.h"
#include "hash_db.h"
#include "index_stat.h"
#include "simple_tokenizer.h"
#include "term.h"
#include "tokenizer.h"

namespace yas {
class InvertFieldsIndexWriter : public FieldIndexWriter,
                                public InvertFieldsIndexReader {
  struct JumptTableEntry {
    uint32_t max_docid;
    uint64_t posting_list_offset;
    uint64_t position_list_offset;
  } __attribute__((packed));

 public:
  InvertFieldsIndexWriter(IndexStat* index_stat);
  InvertFieldsIndexWriter(std::unique_ptr<Tokenizer> tokenizer);
  virtual ~InvertFieldsIndexWriter();
  void flush(FieldInfo fi, uint32_t max_doc,
             const IndexOption& option) override;
  static void flush(std::shared_ptr<DB> db, std::string key,
                    std::vector<uint32_t>& posting_list,
                    std::vector<std::vector<uint32_t>>& positions);
  int add(uint32_t docid, std::shared_ptr<Field> field) override;
  int open() override;
  int close() override;
  std::shared_ptr<TermReader> get_reader(Term term) override;
   DB* get_db() override;

 private:
  std::unique_ptr<Tokenizer> tokenizer_;
  std::unordered_map<std::string, std::vector<uint32_t>> posting_lists_;
  std::unordered_map<std::string, std::vector<std::vector<uint32_t>>>
      position_lists_;
  IndexStat* index_stat_;
};

}  // namespace yas