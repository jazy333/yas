#include "serialized_field_values_reader.h"

namespace yas {
SerializedFieldValuesReader::SerializedFieldValuesReader(
    const std::string& meta_file, const std::string& data_file)
    : meta_file_(meta_file),
      data_file_(data_file),
      field_values_meta_(nullptr),
      field_values_data_(nullptr) {}

SerializedFieldValuesReader::~SerializedFieldValuesReader() {}

int SerializedFieldValuesReader::open() {
  field_values_meta_ = new File(meta_file_, false);
  fdm->open();
  field_values_data_ = new File(data_file_, false);
  fdd->open();
  while (true) {
    int field_id;
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
}

int SerializedFieldValuesReader::close() {
  field_values_meta_->close();
  field_values_data_->close();
  delete field_values_meta_;
  delete field_values_data_;
}

void SerializedFieldValuesReader::read_numeric(const std::string& field_name) {
  NumericFieldMeta* meta = new NumericFieldMeta;

  meta->read(&meta->docids_offset, sizeof(meta->docids_offset));
  meta->read(&meta->docids_len, sizeof(meta->docids_len));
  meta->read(&meta->jump_table_entry_count,
             sizeof(meta->jump_table_entry_count));
  meta->read(&meta->num_values, sizeof(meta->num_values));
  meta->read(&meta->num_bits, sizeof(meta->num_bits));
  meta->read(&meta->min_value, sizeof(meta->min_value));
  if (meta->num_bits != 0) {
    meta->read(&meta->gcd, sizeof(meta->gcd));
    meta->read(&meta->field_values_data_offset,
               sizeof(meta->field_values_data_offset));
    meta->read(&meta->field_values_data_len,
               sizeof(meta->field_values_data_len));
  }
  field_index_metas_[field_name] = meta;
}

void SerializedFieldValuesReader::read_binary(const std::string& field_name) {
  BinaryFieldMeta meta = new BinaryFieldMeta();
  meta->read(&meta->docids_offset, sizeof(meta->docids_offset));
  meta->read(&meta->docids_len, sizeof(meta->docids_len));
  meta->read(&meta->jump_table_entry_count,
             sizeof(meta->jump_table_entry_count));
  meta->read(&meta->num_values, sizeof(meta->num_values));
  meta->read(&meta->min_len, sizeof(meta->min_len));
  meta->read(&meta->max_len, sizeof(meta->max_len));
  meta->read(&meta->field_values_data_offset,
             sizeof(meta->field_values_data_offset));
  meta->read(&meta->field_values_data_len, sizeof(meta->field_values_data_len));
  meta->read(&meta->field_lengths_num_bits,
             sizeof(meta->field_lengths_num_bits));
  meta->read(&meta->field_lengths_data_offset,
             sizeof(meta->field_lengths_data_offset));
  meta->read(&meta->field_lengths_data_len,
             sizeof(meta->field_lengths_data_len));
}

FieldValueIndexReader* SerializedFieldValuesReader::get(
    const std::string& field_name) {
  FieldIndexMeta* meta = nullptr;
  if (field_index_metas_.count(field_name) == 0) {
    return nullptr;
  } else {
    meta = field_index_metas_[field_name];
  }

  switch (meta->type) {
    case 0: {
      return new NumericFieldIndexReader(
          std::dynammic_cast<NumericFieldMeta*>(meta), field_values_data_);
      break;
    }
    case 1: {
      return new BinaryFieldIndexReader(
          std::dynammic_cast<BinaryFieldFieldMeta*>(meta), field_values_data_);
      break;
    }
    default: {
      return nullptr;
      break;
    }
  }
}

}  // namespace yas