#include "point_field_index_writer.h"
#include "memory_point_field_index_reader.h"

namespace yas {
PointFieldIndexWriter::PointFieldIndexWriter(File* kdm, File* kdi, File* kdd)):kdm_(kdm),kdi_(kdi),kdd_(kdd) {}
PointFieldIndexWriter::~PointFieldIndexWriter() {}
void PointFieldIndexWriter::flush(FieldInfo fi, uint32_t max_doc) {
  bkd_.pack(fi.get_field_id(), &points_, kdm_, kdi_, kdd_);
}

void PointFieldIndexWriter::add(uint32_t docid, Field* field) {
  auto point = filed->get_value();
  for (int i = 0; i < point.dim(); ++i) {
    skip_list[i].insert(point.get(i), docid);
  }
  point.set_docid(docid);
  points_.write(point);
}

FieldIndexReader* get_reader() {
  return new MemoryPointFieldIndexReader(&points_);
}
}  // namespace yas