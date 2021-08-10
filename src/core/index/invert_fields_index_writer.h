#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "field_index_reader.h"
#include "field_index_writer.h"
#include "hash_db.h"
#include "term.h"
#include "tokenizer.h"

namespace yas {
class InvertFieldsIndexWriter : public FieldIndexWriter,
                               public InvertFieldIndexReader {
 public:
  InvertFieldsIndexWriter();
  virtual ~InvertFieldsIndexWriter();
  void flush(FieldInfo fi, uint32_t max_doc,const IndexWriterOption& option) override;
  void add(uint32_t docid, Field* field) override;
  int open() override;
  int close() override;
  TermReader* get_reader(Term* term) override;

 private:
  Tokenizer* tokenizer_;
  std::unordered_map<std::string, std::vector<uint32_t>> posting_lists_;
  std::unordered_map<std::string, std::vector<std::vector<int>>>
      position_lists_;
};

}  // namespace yas