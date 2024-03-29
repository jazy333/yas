#pragma once
#include <string>

#include "db.h"
#include "field_index_reader.h"
#include "term.h"
#include "term_reader.h"

namespace yas {
class SerializedInvertFieldsIndexReader : public InvertFieldsIndexReader {
 public:
  SerializedInvertFieldsIndexReader(const std::string& invert_index_file);
  virtual ~SerializedInvertFieldsIndexReader();
  std::shared_ptr<TermReader> get_reader(Term term) override;
  DB* get_db() override;
  int open() override;
  int close() override;

 private:
  std::string invert_index_file_;
  DB* db_;
};
}  // namespace yas