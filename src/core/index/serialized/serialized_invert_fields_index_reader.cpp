#include "serialized_invert_fields_index_reader.h"

#include "block_term_reader.h"
#include "hash_db.h"

namespace yas {
SerializedInvertFieldsIndexReader::SerializedInvertFieldsIndexReader(
    const std::string& invert_index_file)
    : invert_index_file_(invert_index_file), db_(nullptr) {}

SerializedInvertFieldsIndexReader::~SerializedInvertFieldsIndexReader() {
  close();
}

std::shared_ptr<TermReader> SerializedInvertFieldsIndexReader::get_reader(
    Term term) {
  if (db_) {
    auto term_reader =
        std::shared_ptr<BlockTermReader>(new BlockTermReader(db_, term));
    return term_reader;
  } else
    return nullptr;
}

int SerializedInvertFieldsIndexReader::open() {
  db_ = new HashDB();
  return db_->open(invert_index_file_,false);
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

DB* SerializedInvertFieldsIndexReader::get_db() { return db_; }

}  // namespace yas