#pragma once
#include "field_index_writer.h"
#include "tokenizer.h"
#include "hash_db.h"

#include <unordered_map>
#include <vector>

namespace yas {
class InvertFieldIndexWriter : public FieldIndexWriter {
 public:
  InvertFieldIndexWriter();
  virtual ~InvertFieldIndexWriter();
  virtual void flush() override;
  virtual void add(uint32_t docid, Field* field) override;
  virtual FieldIndexReader* get_reader() override;

 private:
  Tokenizer* tokenizer_;
  std::unordered_map<std::string,std::vector<uint32_t>> posting_lists_;
  std::unordered_map<std::string ,std::vector<std::vector<int>>> position_lists_;
};

}  // namespace yas