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

PointFieldIndexReader* SerializedPointFieldsIndexReader::get_reader(
    const std::string& field_name, PointFieldIndexReader* init_reader) {
  if (field_infos_.count(field_name) == 1) {
    int field_id = field_infos_[field_name].get_field_id();
    init_reader->init(field_id, kdm_infos_[field_id], kdi_, kdd_);
    return init_reader;
  } else {
    return nullptr;
  }
}

int SerializedPointFieldsIndexReader::open() {
  if (meta_file_.empty() || index_file_.empty() || data_file_.empty())
    return -1;
  kdm_ = std::shared_ptr<MMapFile>(new MMapFile());
  int ret = kdm_->open(meta_file_, false);
  if (ret < 0) return ret;
  kdi_ = std::shared_ptr<MMapFile>(new MMapFile());
  ret = kdi_->open(index_file_, false);
  if (ret < 0) return ret;
  kdd_ = std::shared_ptr<MMapFile>(new MMapFile());
  ret = kdd_->open(data_file_, false);
  if (ret < 0) return ret;
  loff_t off = 0;
  while (true) {
    PointFieldMeta pfm;
    int ret = kdm_->read_vint(pfm.field_id_, &off);
    if (ret < 0) break;
    if (pfm.field_id_ == -1) break;
    ret = kdm_->read_vint(pfm.num_dims_, &off);
    if (ret < 0) break;
    ret = kdm_->read_vint(pfm.max_count_per_leaf_, &off);
    if (ret < 0) break;
    ret = kdm_->read_vint(pfm.bytes_per_dim_, &off);
    if (ret < 0) break;
    ret = kdm_->read_vint(pfm.num_leaves_, &off);
    if (ret < 0) break;
    int bytes_len = pfm.num_dims_ * pfm.bytes_per_dim_;
    pfm.min_ = new u_char[bytes_len]();
    ret = kdm_->read(pfm.min_, bytes_len, &off);
    if (ret < 0) break;
    pfm.max_ = new u_char[bytes_len]();
    ret = kdm_->read(pfm.max_, bytes_len, &off);
    if (ret < 0) break;
    ret = kdm_->read_vint(pfm.count_, &off);
    if (ret < 0) break;
    ret = kdm_->read_vint(pfm.data_fp_, &off);
    if (ret < 0) break;
    ret = kdm_->read_vint(pfm.index_fp_, &off);
    if (ret < 0) break;
    ret = kdm_->read_vint(pfm.data_size, &off);
    if (ret < 0) break;
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

PointFieldMeta* SerializedPointFieldsIndexReader::get_meta(int field_id) {
  if (kdm_infos_.count(field_id) == 1)
    return &kdm_infos_[field_id];
  else
    return nullptr;
}

}  // namespace yas