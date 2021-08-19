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

PointFieldIndexReader*
SerializedPointFieldsIndexReader::get_reader(
    const std::string& field_name,
   PointFieldIndexReader* init_reader) {
  if (field_infos_.count(field_name) == 1) {
    int field_id = field_infos_[field_name].get_field_id();
    init_reader->init(field_id, kdm_infos_[field_id], kdi_, kdm_);
    return init_reader;
  } else {
    return nullptr;
  }
}

int SerializedPointFieldsIndexReader::open() {
  kdm_ = new MMapFile(;
  kdm_->open(files_.kdm, false));
  kdi_ = new MMapFile();
  kdi_->open(files_.kdi, false);
  kdd_ = new MMapFile();
  kdd_->open(files_.kdd, false);
  loff_t off = 0;
  while (true) {
    PointFieldMeta pfm;
    int ret = kdm_->read_vint(pfm.field_id_, &off);
    if (pfm.field_id_ == 0) break;
    ret = kdm_->read_vint(pfm.num_dims_, &off);
    ret = kdm_->read_vint(pfm.max_count_per_leaf_, &off);
    ret = kdm_->read_vint(pfm.bytes_per_dim_, &off);
    ret = kdm_->read_vint(pfm.num_leaves_, &off);
    int bytes_len = pfm.num_dims_ * pfm.bytes_per_dim_;
    ret = kdm_->read(pfm.min_, bytes_len, &off);
    ret = kdm_->read(pfm.max_, bytes_len, &off);
    ret = kdm_->read_vint(pfm.count_, &off);
    ret = kdm_->read_vint(pfm.data_fp_, &off);
    ret = kdm_->read_vint(pfm.index_fp_, &off);
    kdm_infos_[pfm.field_id_] = pfm;
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