#pragma once
#include <cstdint>
#include <memory>
#include <vector>

#include "core/db/db.h"
#include "core/index/field_index_meta.h"
#include "core/db/file.h"
#include "core/tokenizer/term.h"
#include "core/index/term_reader.h"

namespace yas {
class FieldValueIndexReader {
 public:
  virtual void get(uint32_t docid, uint64_t& value) = 0;
  virtual void get(uint32_t docid, std::vector<uint8_t>& value) = 0;
};

class FieldValuesIndexReader {
 public:
  virtual int open() = 0;
  virtual int close() = 0;
  virtual std::shared_ptr<FieldValueIndexReader> get_reader(
      const std::string& field_name) = 0;
};

class PointFieldIndexReader {
 public:
  virtual ~PointFieldIndexReader() = default;
  virtual std::shared_ptr<PostingList> get(u_char* min, u_char* max) = 0;
  virtual void init(int field_id, PointFieldMeta meta,
                    std::shared_ptr<File> kdi, std::shared_ptr<File> kdd) = 0;
};

class PointFieldsIndexReader {
 public:
  virtual int open() = 0;
  virtual int close() = 0;
  virtual PointFieldIndexReader* get_reader(
      const std::string& field_name, PointFieldIndexReader* init_reader) = 0;
  virtual PointFieldMeta* get_meta(int field_id) = 0;
};

class InvertFieldsIndexReader {
 public:
  virtual std::shared_ptr<TermReader> get_reader(Term term) = 0;
  virtual DB* get_db() = 0;
  virtual int open() = 0;
  virtual int close() = 0;
};

}  // namespace yas