#pragma once
#include <cstdint>
#include <memory>
#include <vector>

#include "term_reader.h"

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
  virtual FieldValueIndexReader* get_reader(const std::string& field_name) = 0;
};

class PointFieldIndexReader {
 public:
  virtual PostingList* get(u_char* min, u_char* max) = 0;
  virtual void init(int field_id, PointFieldMeta meta, std::shared_ptr<File> kdi,
                    std::shared_ptr<File> kdd) = 0;
};

class PointFieldsIndexReader {
 public:
  virtual int open() = 0;
  virtual int close() = 0;
  virtual std::unique_ptr<PointFieldIndexReader> get_reader(
      const std::string& field_name,
      std::unique_ptr<PointFieldIndexReader> init_reader) = 0;
};

class InvertFieldsIndexReader {
 public:
  virtual int open() = 0;
  virtual int close() = 0;
  virtual TermReader* get_reader(Term* term) = 0;
};

}  // namespace yas