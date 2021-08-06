#include "point_fields_index_reader.h"

namespace yas {
PointFieldsIndexReader::PointFieldsIndexReader(FieldInfo* field_infos,
                                               File* kdm, File* kdi,
                                               File* kdd) {}
PointFieldsIndexReader::~PointFieldsIndexReader() {}
int PointFieldsIndexReader::read_meta() {
  loff_t off = 0;
  while (true) {
    MetaFieldInfo mfi;
    int ret = kdm_->read_vint(mfi.field_id_, &off);
    if (mfi.field_id_ == 0) break;
    ret = kdm_->read_vint(mfi.num_dims_, &off);
    ret = kdm_->read_vint(mfi.max_count_per_leaf_, &off);
    ret = kdm_->read_vint(mfi.bytes_per_dim_, &off);
    ret = kdm_->read_vint(mfi.num_leaves_, &off);
    u_char* min_data = mfi.min_.bytes();
    u_char* max_data = mfi.max_.bytes();
    int bytes_len = mfi.num_dims_ * mfi.bytes_per_dim_;
    ret = kdm_->read(min_data, bytes_len, &off);
    ret = kdm_->read(max_data, bytes_len, &off);
    ret = kdm_->read_vint(mfi.count_, &off);
    ret = kdm_->read_vint(mfi.data_fp_, &off);
    ret = kdm_->read_vint(mfi.index_fp_, &off);
    kdm_infos_[mfi.field_id_] = mfi;
  }
}
}  // namespace yas