#include "index_writer.h"

namespace {
IndexWriter::IndexWriter(/* args */) {}

IndexWriter::~IndexWriter() {}

void IndexWriter::add_document(Document* doc) {
  std::vector<Field*> fields = doc->get_fields();
  for (auto field : fields) {
    if (field_index_writers_.count(field->get_name()) == 1) {
      field_index_writers_[field->get_name()]->add(field);
    } else {
      FieldIndexWriter* field_index_writer = field->get_field_index_writer();
      field_index_writer->add(field);
      field_index_writers_[field->get_name()] = field_index_writer;
    }
  }
}
void IndexWriter::flush() {
  for (auto field_index_writer : field_index_writers_) {
    field_index_writer->flush();
  }
}

}  // namespace