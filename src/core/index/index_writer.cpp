#include "index_writer.h"

#include <ctime>

#include "binary_field_index_writer.h"
#include "common/common.h"
#include "log.h"
#include "memory_index_reader.h"
#include "numeric_field.h"
#include "numeric_field_index_writer.h"

namespace yas {
IndexWriter::IndexWriter()
    : max_doc_(1),
      max_field_num_(1),
      option_(IndexOption()),
      tokenizer_(std::shared_ptr<Tokenizer>(new SimpleTokenizer())) {
  index_stat_.doc_count = 0;
  index_stat_.max_doc = 0;
  index_stat_.total_term_freq = 0;
}

IndexWriter::IndexWriter(const IndexOption& index_writer_option)
    : max_doc_(1),
      max_field_num_(1),
      option_(index_writer_option),
      tokenizer_(std::shared_ptr<Tokenizer>(new SimpleTokenizer())) {
  index_stat_.doc_count = 0;
  index_stat_.max_doc = 0;
  index_stat_.total_term_freq = 0;
}

IndexWriter::IndexWriter(const IndexOption& index_writer_option,
                         std::shared_ptr<Tokenizer> tokenizer)
    : max_doc_(1),
      max_field_num_(1),
      option_(index_writer_option),
      tokenizer_(tokenizer) {
  index_stat_.doc_count = 0;
  index_stat_.max_doc = 0;
  index_stat_.total_term_freq = 0;
}

IndexWriter::~IndexWriter() {
  for (auto&& kv : point_fields_index_writers_) {
    FieldIndexWriter* writer = kv.second;
    delete writer;
  }
  point_fields_index_writers_.clear();
  point_fields_index_readers_.clear();
}

int IndexWriter::open() {
  option_.read_stat(index_stat_);
  max_field_num_ = option_.read_field_info(field_infos_);
  option_.current_segment_no=index_stat_.max_seg_no;
  return 0;
}

void IndexWriter::process_numeric_field(const std::string& field_name,
                                        std::shared_ptr<Field> field) {
  std::shared_ptr<FieldIndexWriter> field_index_writer = nullptr;
  if (field_values_index_writers_.count(field_name) == 1) {
    field_index_writer = field_values_index_writers_[field_name];

  } else {
    std::shared_ptr<NumericFieldIndexWriter> numeric_index_writer =
        std::shared_ptr<NumericFieldIndexWriter>(new NumericFieldIndexWriter());
    field_values_index_writers_[field_name] = numeric_index_writer;
    field_values_index_readers_[field_name] = numeric_index_writer;
  }
  field_index_writer = field_values_index_writers_[field_name];
  field_index_writer->add(max_doc_, field);
}

void IndexWriter::add_document(std::unique_ptr<Document> doc) {
  std::lock_guard<SharedMutex> lock(shared_mutex_);
  std::vector<std::shared_ptr<Field>> fields = doc->get_fields();
  for (auto field : fields) {
    int index_type = field->get_index_type();
    std::string field_name = field->get_name();
    if (field_name.find("__") == 0) {
      LOG_WARN("field name should not begin with __(reserved),field_name=%s",
               field_name.c_str());
      continue;
    }

    if (field_infos_.count(field_name) == 0) {
      FieldInfo fi;
      fi.set_field_id(max_field_num_++);
      fi.set_field_name(field_name);
      fi.set_index_type(index_type);
      field_infos_[field_name] = fi;
    }

    switch (index_type) {
      case 0: {
        // no index
        break;
      }
      case 1: {  // invert
        if (!invert_fields_writer_) {
          invert_fields_writer_ = std::shared_ptr<InvertFieldsIndexWriter>(
              new InvertFieldsIndexWriter(&index_stat_, tokenizer_));
          invert_fields_reader_ = invert_fields_writer_;
        }
        int doc_len = invert_fields_writer_->add(max_doc_, field);

        // add norm doc len field
        std::string norm_field_name = "__" + field->get_name() + "_dl";
        if (field_infos_.count(norm_field_name) == 0) {
          FieldInfo fi;
          fi.set_field_id(max_field_num_++);
          fi.set_field_name(norm_field_name);
          fi.set_index_type(2);
          field_infos_[norm_field_name] = fi;
        }
        long norm_doc_len = uint2uchar(doc_len);
        // LOG_INFO("docid=%u,doc_len=%d,norm_doc_len=%ld",max_doc_.load(),doc_len,norm_doc_len);
        auto dl_field =
            std::make_shared<NumericField>(norm_field_name, norm_doc_len);
        process_numeric_field(norm_field_name, dl_field);
        break;
      }
      case 2: {  // numeric
        process_numeric_field(field_name, field);
        break;
      }
      case 3: {  // binary
        std::shared_ptr<FieldIndexWriter> field_index_writer = nullptr;
        if (field_values_index_writers_.count(field_name) == 0) {
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
        LOG_WARN("not support type,type=%d", index_type);
        break;
      }
    }
  }
  index_stat_.doc_count++;
  index_stat_.max_doc = max_doc_.load();

  ++max_doc_;
}

int IndexWriter::write_segment_info() {
  auto segment_file_handle = std::unique_ptr<File>(new MMapFile);
  std::string file_si = option_.get_segment_info_file();
  segment_file_handle->open(file_si, true, true);
  uint32_t max_doc = max_doc_.load() - 1;
  segment_file_handle->append(&max_doc, sizeof(max_doc));
  time_t now = time(nullptr);
  segment_file_handle->append(&now, sizeof(now));
  segment_file_handle->close();
  return 0;
}

void IndexWriter::flush() {
  std::lock_guard<SharedMutex> lock(shared_mutex_);
  if (invert_fields_writer_) {
    FieldInfo dummy;
    invert_fields_writer_->flush(dummy, max_doc_-1, option_);
    invert_fields_writer_->close();
    invert_fields_writer_->open();
  }

  for (auto kv : point_fields_index_writers_) {
    auto field_name = kv.first;
    auto field_index_writer = kv.second;
    FieldInfo field_info = field_infos_[field_name];
    field_index_writer->flush(field_info, max_doc_-1, option_);
    delete field_index_writer;
  }
  point_fields_index_writers_.clear();

  for (auto kv : field_values_index_writers_) {
    auto field_name = kv.first;
    auto field_index_writer = kv.second;
    FieldInfo field_info = field_infos_[field_name];
    field_index_writer->flush(field_info, max_doc_-1, option_);
  }
  field_values_index_writers_.clear();
  index_stat_.max_seg_no = option_.current_segment_no + 1;
  option_.write_stat(index_stat_);
  option_.write_field_info(field_infos_);

  write_segment_info();

  option_.current_segment_no++;

  max_doc_ = 1;
}

std::unique_ptr<SubIndexReader> IndexWriter::get_sub_index_reader() {
  return std::unique_ptr<MemoryIndexReader>(
      new MemoryIndexReader(invert_fields_reader_, &point_fields_index_readers_,
                            &field_values_index_readers_));
}

}  // namespace yas