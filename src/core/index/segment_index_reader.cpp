#include "segment_index_reader.h"

namespace yas {
SegmentIndexReader::SegmentIndexReader(SegmentFiles files) : files_(files) {}
SegmentIndexReader::~SegmentIndexReader() {}
TermReader* SegmentIndexReader::posting_list(Term* term) {
  return BlockTermReader(db_, term);
}
FieldIndexReadr* SegmentIndexReader::field_reader(std::string field_name) {
  field_values_reader_.get(field_name);
}
PointFieldIndexReader* point_reader(std::string field_name) {
  return point_reader_.get_reader(field_name);
}
int SegmentIndexReader::open() {
  File* kdm = new File(files_.kdm, false);
  kdm->open();
  File* kdi = new File(files_.kdi, false);
  kdi->open();
  File* kdd = new File(files_.kdd, false);
  kdd->open();
  point_reader_ = new PointFieldsIndexReader(kdm, kdi, kdd);
  point_reader_->open();

  db_ = new HashDB(files_.invert_index_file);
  db_->open();

  File* fdm = new File(files_.fdm, false);
  kdm->open();
  File* fdd = new File(files_.fdd, false);
  field_values_reader_ = new FieldValuesReadr(fdm, fdd);
  field_values_reader_->open();
}
void SegmentIndexReader::close() {}

}  // namespace yas