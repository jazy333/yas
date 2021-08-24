#include "serialized_field_values_index_reader.h"

#include "binary_field_index_reader.h"
#include "mmap_file.h"
#include "numeric_field_index_reader.h"

namespace yas {
SerializedFieldValuesIndexReader::SerializedFieldValuesIndexReader(
    const std::string& meta_file, const std::string& data_file)
    : meta_file_(meta_file),
      data_file_(data_file),
      field_values_meta_(nullptr),
      field_values_data_(nullptr) {}

SerializedFieldValuesIndexReader::~SerializedFieldValuesIndexReader() {
  close();
  for (auto&& item : field_index_metas_) {
    delete item.second;
  }
}

int SerializedFieldValuesIndexReader::open() {
  field_values_meta_ = new MMapFile();
  field_values_meta_->open(meta_file_, false);
  field_values_data_ = new MMapFile();
  field_values_data_->open(meta_file_, false);
  while (true) {
    int field_id = -1;
    field_values_meta_->read(&field_id, sizeof(field_id));
    if (field_id == -1) {
      break;
    }
    int8_t type;
    std::string field_name;
    field_values_meta_->read(&type, sizeof(type));
    switch (type) {
      case 0: {
        read_numeric(field_name);
        break;
      }
      case 1: {
        read_binary(field_name);
        break;
      }
      default:
        break;
    }
  }
  return 0;
}

int SerializedFieldValuesIndexReader::close() {
  field_values_meta_->close();
  field_values_data_->close();
  delete field_values_meta_;
  delete field_values_data_;
  return 0;
}

void SerializedFieldValuesIndexReader::read_numeric(
    const std::string& field_name) {
  NumericFieldMeta* meta = new NumericFieldMeta;

  field_values_meta_->read(&meta->docids_offset, sizeof(meta->docids_offset));
  field_values_meta_->read(&meta->docids_length, sizeof(meta->docids_length));
  field_values_meta_->read(&meta->jump_table_entry_count,
                           sizeof(meta->jump_table_entry_count));
  field_values_meta_->read(&meta->num_values, sizeof(meta->num_values));
  field_values_meta_->read(&meta->num_bits, sizeof(meta->num_bits));
  field_values_meta_->read(&meta->min_value, sizeof(meta->min_value));
  if (meta->num_bits != 0) {
    field_values_meta_->read(&meta->gcd, sizeof(meta->gcd));
    field_values_meta_->read(&meta->field_values_data_offset,
                             sizeof(meta->field_values_data_offset));
    field_values_meta_->read(&meta->field_values_data_len,
                             sizeof(meta->field_values_data_len));
  }
  field_index_metas_[field_name] = meta;
}

void SerializedFieldValuesIndexReader::read_binary(
    const std::string& field_name) {
  BinaryFieldMeta* meta = new BinaryFieldMeta();
  field_values_meta_->read(&meta->docids_offset, sizeof(meta->docids_offset));
  field_values_meta_->read(&meta->docids_length, sizeof(meta->docids_length));
  field_values_meta_->read(&meta->jump_table_entry_count,
                           sizeof(meta->jump_table_entry_count));
  field_values_meta_->read(&meta->num_values, sizeof(meta->num_values));
  field_values_meta_->read(&meta->min_len, sizeof(meta->min_len));
  field_values_meta_->read(&meta->max_len, sizeof(meta->max_len));
  field_values_meta_->read(&meta->field_values_data_offset,
                           sizeof(meta->field_values_data_offset));
  field_values_meta_->read(&meta->field_values_data_len,
                           sizeof(meta->field_values_data_len));
  if (meta->min_len != meta->max_len) {
    field_values_meta_->read(&meta->field_lengths_num_bits,
                             sizeof(meta->field_lengths_num_bits));
    field_values_meta_->read(&meta->field_lengths_data_offset,
                             sizeof(meta->field_lengths_data_offset));
    field_values_meta_->read(&meta->field_lengths_data_len,
                             sizeof(meta->field_lengths_data_len));
  }
}

std::shared_ptr<FieldValueIndexReader>
SerializedFieldValuesIndexReader::get_reader(const std::string& field_name) {
  FieldIndexMeta* meta = nullptr;
  if (field_index_metas_.count(field_name) == 0) {
    return nullptr;
  } else {
    meta = field_index_metas_[field_name];
  }

  switch (meta->type) {
    case 0: {
      return std::shared_ptr<FieldValueIndexReader>(new NumericFieldIndexReader(
          reinterpret_cast<NumericFieldMeta*>(meta), field_values_data_));
      break;
    }
    case 1: {
      return std::shared_ptr<FieldValueIndexReader>(new BinaryFieldIndexReader(
          reinterpret_cast<BinaryFieldMeta*>(meta), field_values_data_));
      break;
    }
    default: {
      return nullptr;
      break;
    }
  }
}

}  // namespace yas