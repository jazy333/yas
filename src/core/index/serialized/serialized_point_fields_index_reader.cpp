#include "serialized_point_fields_index_reader.h"
#include "serialized_point_field_index_reader.h"

namespace yas {
SerializedPointFieldsIndexReader::SerializedPointFieldsIndexReader(
    const std::unordered_map<std::string, FieldInfo>& field_infos,
    const std::string& meta_file, const std::string& index_file,
    const std::string& data_file)
    : field_infos_(field_infos),
      meta_file_(meta_file),
      index_file_(index_file),
      data_file_(data_file),
      kdm_(nullptr),
      kdi_(nullptr),
      kdd_(nullptr) {}

SerializedPointFieldsIndexReader::~SerializedPointFieldsIndexReader() {
  close();
}

std::unique_ptr<PointFieldIndexReader>
SerializedPointFieldsIndexReader::get_reader(
    const std::string& field_name,
    std::unique_ptr<PointFieldIndexReader> init_reader) {
  if (field_infos_.count(field_name) == 1) {
    int field_id = field_infos_[field_name].get_field_id();
    init_reader->init(field_id, kdm_infos_[field_id], kdi_, kdm_);
    return init_reader;
  } else {
    return nullptr;
  }
}

int SerializedPointFieldsIndexReader::open() {
  kdm_ = new MMapFile(files_.kdm, false);
  kdm_->open();
  kdi_ = new MMapFile(files_.kdi, false);
  kdi_->open();
  kdd_ = new MMapFile(files_.kdd, false);
  kdd_->open();
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
  return 0;
}

int SerializedPointFieldsIndexReader::close() {
  if (kdm_) kdm_->close();
  if (kdi_) kdi_->close();
  if (kdd_) kdd_->close();
  return 0;
}
}  // namespace yas