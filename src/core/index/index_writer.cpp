#include "index_writer.h"

#include <mutex>

#include "memory_index_reader.h"
namespace {
IndexWriter::IndexWriter() : option_(IndexOption()) {}

IndexWriter::IndexWriter(const IndexOption& index_writer_option)
    : options_(index_writer_option) {}

IndexWriter::~IndexWriter() {}

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
        if (!invert_fields_writer_)
          invert_fields_writer_ = new InvertFieldsIndexWriter();
        invert_fields_writer_->add(max_doc_, field);
        break;
      }
      case 2: {  // numeric
        std::shared_ptr<FieldIndexWriter> field_index_writer = nullptr;
        if (field_values_index_writers_.count(field_name) == 1) {
          field_index_writer = field_values_index_writers_[field_name];

        } else {
          field_index_writer = new NumericFieldIndexWriter();
          field_values_index_writers_[field_name] = field_index_writer;
        }
        field_index_writer->add(max_doc_, field);
        break;
      }
      case 3: {  // binary
        std::shared_ptr<FieldIndexWriter> field_index_writer = nullptr;
        if (field_values_index_writers_.count(field_name) == 1) {
          field_index_writer = field_values_index_writers_[field_name];
        } else {
          field_index_writer = new BinaryFieldIndexWriter();
          field_values_index_writers_[field_name] = field_index_writer;
        }
        field_index_writer->add(max_doc_, field);
        break;
      }
      case 4: {  // point
        std::shared_ptr<FieldIndexWriter> field_index_writer = nullptr;
        if (field_values_index_writers_.count(field_name) == 1) {
          field_index_writer = field_values_index_writers_[field_name];
        } else {
          field_index_writer = field->make_field_index_writer();
          field_values_index_writers_[field_name] = field_index_writer;
        }
        field_index_writer->add(max_doc_, field);
        break;
      }
      default: {
        // not support
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
  invert_fields_writer->close();
  for (auto kv : point_fields_index_writers_) {
    auto field_name = kv.first;
    auto field_index_writer = kv.second;
    FieldInfo field_info = field_infos_[field_name];
    field_index_writer->flush(field_info, max_doc_, option_);
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

std::unique_ptr<MemoryIndexReader> IndexWriter::get_sub_index_reader() {
  return new std::unique_ptr<MemoryIndexReader>(invert_fields_writer_,
                                                &point_fields_index_writers_,
                                                &field_values_index_writers_);
}

}  // namespace