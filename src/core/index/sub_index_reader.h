#pragma once
#include <memory>
#include <string>

#include "field_index_reader.h"
#include "posting_list.h"
#include "term.h"
#include "term_reader.h"
#include "segment_info.h"

namespace yas {
class SubIndexReader {
 public:
  virtual std::shared_ptr<InvertFieldsIndexReader> invert_index_reader() = 0;
  virtual std::shared_ptr<FieldValuesIndexReader> field_values_reader() = 0;
  virtual std::shared_ptr<PointFieldsIndexReader> point_fields_reader() = 0;
  virtual int open() = 0;
  virtual int close() = 0;
  virtual SegmentInfo get_segment_info()=0;
};
}  // namespace yas