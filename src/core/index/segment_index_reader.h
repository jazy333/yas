#include "block_term_reader.h"
#include "field_values_reader.h"
#include "point_fields_index_reader.h"
#include "segment_files.h"
#include "sub_index_reader.h"

namespace yas {
class SegmentIndexReader : public SubIndexReader {
 public:
  SegmentIndexReader(SegmentFiles files);
  virtual ~SegmentIndexReader();
  TermReader* posting_list(Term*) override;
  FieldIndexReader* field_reader(std::string field_name) override;
  PointFieldIndexReader* point_reader(std::string field_name);
  int open();
  int close();

 private:
  SegmentFiles files_;
  PointFieldsIndexReader* point_reader_;
  FieldValuesReader* field_values_reader_;
  DB* db_;
};
}  // namespace yas