#include "serialized_invert_fields_index_reader.h"
#include "block_term_reader.h"

namespace yas {
SerializedInvertFieldsIndexReader::SerializedInvertFieldsIndexReader(
    const std::string& invert_index_file)
    : invert_index_file_(invert_index_file), db_(nullptr) {}

SerializedInvertFieldsIndexReader::~SerializedInvertFieldsIndexReader() {
  close();
}

TermReader* SerializedInvertFieldsIndexReader::get_reader(Term* term) {
  if (db_)
    return BlockTermReader(db_, term);
  else
    return nullptr;
}

int SerializedInvertFieldsIndexReader::open() {
  db_ = new HashDB(files_.invert_index_file);
  return db_->open();
}

int SerializedInvertFieldsIndexReader::close() {
  int ret = 0;
  if (db_) {
    ret = db_->close();
    delete db_;
    db_ = nullptr;
  }
  return ret;
}

}  // namespace yas