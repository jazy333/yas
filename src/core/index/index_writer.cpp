#include "index_writer.h"

#include "binary_field_index_writer.h"
#include "memory_index_reader.h"
#include "numeric_field_index_writer.h"
#include "log.h"

namespace yas {
IndexWriter::IndexWriter() : option_(IndexOption()) {}

IndexWriter::IndexWriter(const IndexOption& index_writer_option)
    : option_(index_writer_option) {}

IndexWriter::~IndexWriter() {
  for (auto&& kv : point_fields_index_writers_) {
    FieldIndexWriter* writer = kv.second;
    delete writer;
  }
  point_fields_index_writers_.clear();
  point_fields_index_readers_.clear();
}

void IndexWriter::add_document(std::unique_ptr<Document> doc) {
  std::lock_guard<SharedMutex> lock(shared_mutex_);
  std::vector<std::shared_ptr<Field>> fields = doc->get_fields();
  for (auto field : fields) {
    int index_type = field->get_index_type();
    std::string field_name = field->get_name();
    switch (index_type) {
      case 0: {
        // no index
        break;
      }
      case 1: {  // invert
        if (!invert_fields_writer_) {
          invert_fields_writer_ = std::shared_ptr<InvertFieldsIndexWriter>(
              new InvertFieldsIndexWriter());
          invert_fields_reader_ = invert_fields_writer_;
        }
        invert_fields_writer_->add(max_doc_, field);
        break;
      }
      case 2: {  // numeric
        std::shared_ptr<FieldIndexWriter> field_index_writer = nullptr;
        if (field_values_index_writers_.count(field_name) == 1) {
          field_index_writer = field_values_index_writers_[field_name];

        } else {
          std::shared_ptr<NumericFieldIndexWriter> numeric_index_writer =
              std::shared_ptr<NumericFieldIndexWriter>(
                  new NumericFieldIndexWriter());
          field_values_index_writers_[field_name] = numeric_index_writer;
          field_values_index_readers_[field_name] = numeric_index_writer;
        }
        field_index_writer = field_values_index_writers_[field_name];
        field_index_writer->add(max_doc_, field);

        break;
      }
      case 3: {  // binary
        std::shared_ptr<FieldIndexWriter> field_index_writer = nullptr;
        if (field_values_index_writers_.count(field_name) == 1) {
        } else {
          std::shared_ptr<BinaryFieldIndexWriter> binary_index_writer =
              std::shared_ptr<BinaryFieldIndexWriter>(
                  new BinaryFieldIndexWriter());
          field_values_index_writers_[field_name] = binary_index_writer;
          field_values_index_readers_[field_name] = binary_index_writer;
        }
        field_index_writer = field_values_index_writers_[field_name];
        field_index_writer->add(max_doc_, field);
        break;
      }
      case 4: {  // point
        FieldIndexWriter* field_index_writer = nullptr;
        if (point_fields_index_writers_.count(field_name) == 1) {
          field_index_writer = point_fields_index_writers_[field_name];
        } else {
          field_index_writer = field->make_field_index_writer();
          point_fields_index_writers_[field_name] = field_index_writer;
          point_fields_index_readers_[field_name] =
              reinterpret_cast<PointFieldIndexReader*>(field_index_writer);
        }
        field_index_writer->add(max_doc_, field);
        break;
      }
      default: {
        // not support
        LOG_WARN("not support type,type=%d",index_type);
        break;
      }
    }
  }
  ++max_doc_;
}

void IndexWriter::flush() {
  std::lock_guard<SharedMutex> lock(shared_mutex_);
  FieldInfo dummy;
  invert_fields_writer_->flush(dummy, max_doc_, option_);
  invert_fields_writer_->close();
  invert_fields_writer_->open();

  for (auto kv : point_fields_index_writers_) {
    auto field_name = kv.first;
    auto field_index_writer = kv.second;
    FieldInfo field_info = field_infos_[field_name];
    field_index_writer->flush(field_info, max_doc_, option_);
    delete field_index_writer;
  }
  point_fields_index_writers_.clear();

  for (auto kv : field_values_index_writers_) {
    auto field_name = kv.first;
    auto field_index_writer = kv.second;
    FieldInfo field_info = field_infos_[field_name];
    field_index_writer->flush(field_info, max_doc_, option_);
  }
  field_values_index_writers_.clear();

  option_.current_segment_no++;
  max_doc_ = 1;
}

std::unique_ptr<SubIndexReader> IndexWriter::get_sub_index_reader() {
  return std::unique_ptr<MemoryIndexReader>(
      new MemoryIndexReader(invert_fields_reader_, &point_fields_index_readers_,
                            &field_values_index_readers_));
}

}  // namespace yas