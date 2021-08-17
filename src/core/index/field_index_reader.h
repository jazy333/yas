#pragma once
#include <cstdint>
#include <memory>
#include <vector>

#include "field_index_meta.h"
#include "file.h"
#include "term_reader.h"
#include "term.h"

namespace yas {
class FieldValueIndexReader {
 public:
  virtual void get(uint32_t docid, uint64_t& value) = 0;
  virtual void get(uint32_t docid, std::vector<char>& value) = 0;
};

class FieldValuesIndexReader {
 public:
  virtual int open() = 0;
  virtual int close() = 0;
  virtual std::shared_ptr<FieldValueIndexReader> get_reader(const std::string& field_name) = 0;
};

class PointFieldIndexReader {
 public:
  virtual PostingList* get(u_char* min, u_char* max) = 0;
  virtual void init(int field_id, PointFieldMeta meta,
                    std::shared_ptr<File> kdi, std::shared_ptr<File> kdd) = 0;
};

class PointFieldsIndexReader {
 public:
  virtual int open() = 0;
  virtual int close() = 0;
  virtual std::shared_ptr<PointFieldIndexReader> get_reader(
      const std::string& field_name,
      std::shared_ptr<PointFieldIndexReader> init_reader) = 0;
};

class InvertFieldsIndexReader {
 public:
  virtual TermReader* get_reader(Term* term) = 0;
  virtual int open() = 0;
  virtual int close() = 0;
};

}  // namespace yas