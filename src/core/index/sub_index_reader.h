#pragma once
#include <memory>
#include <string>

#include "core/index/field_index_reader.h"
#include "core/search/match/posting_list.h"
#include "core/tokenizer/term.h"
#include "core/index/term_reader.h"
#include "core/index/segment_info.h"
#include "core/index/segment_files.h"

namespace yas {
class SubIndexReader {
 public:
  virtual std::shared_ptr<InvertFieldsIndexReader> invert_index_reader() = 0;
  virtual std::shared_ptr<FieldValuesIndexReader> field_values_reader() = 0;
  virtual std::shared_ptr<PointFieldsIndexReader> point_fields_reader() = 0;
  virtual int open() = 0;
  virtual int close() = 0;
  virtual int unlink(){
    return 0;
  }
  virtual SegmentInfo get_segment_info()=0;
  virtual SegmentFiles get_segment_files()=0;
};
}  // namespace yas