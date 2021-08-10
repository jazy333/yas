#include "index_writer.h"
#include "memory_index_reader.h"

#include <mutex>
namespace {
IndexWriter::IndexWriter() : option_(IndexWriterOption()) {}

IndexWriter::IndexWriter(const IndexWriterOption& index_writer_option)
    : options_(index_writer_option) {}

IndexWriter::~IndexWriter() {}

void IndexWriter::add_document(std::unique_ptr<Document> doc) {
  std::lock_guard<SharedMutex> lock(shared_mutex_);
  std::vector<std::unique_ptr<Field>> fields = doc->get_fields();
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
        std::unique_ptr<FieldIndexWriter> field_index_writer = nullptr;
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
        std::unique_ptr<FieldIndexWriter> field_index_writer = nullptr;
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
        std::unique_ptr<FieldIndexWriter> field_index_writer = nullptr;
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
  invert_fields_writer_->flush(max_doc_, option_);
  invert_fields_writer->close();
  for (auto field_index_writer : point_fields_index_writers_) {
    field_index_writer->flush(max_doc_, option_);
  }
  point_fields_index_writers_.clear();
  for (auto field_index_writer : field_values_index_writers_) {
    field_index_writer->flush(max_doc_, option_);
  }
  field_values_index_writers_.clear();
  option_.current_segment_no++;
  max_doc_ = 1;
}

std::unique_ptr<MemoryIndexReader> IndexWriter::get_sub_index_reader() {
  return new std::unique_ptr<MemoryIndexReader>(&invert_fieldS_writer_,
                                                &point_fields_index_writers_,
                                                &field_values_index_writers_);
}

}  // namespace