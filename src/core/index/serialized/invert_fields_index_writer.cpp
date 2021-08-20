#include "invert_fields_index_writer.h"

#include <iterator>
#include <memory>

#include "memory_file.h"
#include "memory_term_reader.h"
#include "simd_binary_compression.h"
#include "text_field.h"
#include "variable_byte_compression.h"

namespace yas {
InvertFieldsIndexWriter::InvertFieldsIndexWriter() {}

InvertFieldsIndexWriter::~InvertFieldsIndexWriter() {}

void InvertFieldsIndexWriter::flush(FieldInfo fi, uint32_t max_doc,
                                    const IndexOption& option) {
  auto db = std::unique_ptr<HashDB>(new HashDB);
  std::string db_path = option.dir + "/" + option.segment_prefix +
                        std::to_string(option.current_segment_no) + ".hdb";
  db->open(db_path);
  size_t unit_reserve_size = 1024;
  uint8_t* unit = new uint8_t[unit_reserve_size];
  for (auto&& item : posting_lists_) {
    std::string key = item.first;
    auto& docids = item.second;
    auto& positions = position_lists_[key];
    uint32_t max_doc = *(docids.rbegin());
    size_t doc_num = docids.size();
    SIMDBinaryCompression<true> bc_posting_list;
    SIMDBinaryCompression<false> bc_position_length;
    VariableByteCompression<true> vbc_delta;
    VariableByteCompression<true> vbc_no_delta;
    std::vector<uint8_t> compressed_postinglist_buffer;
    std::vector<uint8_t> compressed_positionlist_buffer;
    std::vector<JumptTableEntry> jump_table;
    uint32_t last_unit_docids_compress_size = 0;
    uint32_t last_unit_positions_compress_size = 0;
    int i = 0;
    size_t unit_docid_num = 0;
    for (int i = 0; i < doc_num; i += unit_docid_num) {
      JumptTableEntry entry;
      unit_reserve_size = 1024;
      if (i + 128 <= doc_num) {
        unit_docid_num = 128;
      } else {
        unit_docid_num = doc_num - i;
      }
      entry.max_docid = docids[i + unit_docid_num - 1];
      entry.posting_list_offset = compressed_postinglist_buffer.size();

      if (unit_docid_num == 128) {
        bc_posting_list.compress(docids.data() + i, unit_docid_num, unit,
                                 unit_reserve_size);

        bc_posting_list.set_init(docids[i]);
      } else {
        vbc_delta.compress(docids.data() + i, unit_docid_num, unit,
                           unit_reserve_size);
        last_unit_docids_compress_size = unit_reserve_size;
      }
      compressed_postinglist_buffer.insert(compressed_postinglist_buffer.end(),
                                           unit, unit + unit_reserve_size);
      entry.position_list_offset = compressed_positionlist_buffer.size();
      jump_table.push_back(entry);  // positions list offset
      std::vector<uint32_t> position_lens;
      for (int j = i; j < i + unit_docid_num; ++j) {
        size_t one_doc_positions_len = positions[j].size() * 2 * 4;
        uint8_t* one_doc_positions = new uint8_t[one_doc_positions_len];
        vbc_delta.compress(positions[j].data(), positions[j].size(),
                           one_doc_positions, one_doc_positions_len);
        position_lens.push_back(one_doc_positions_len);
        compressed_positionlist_buffer.insert(
            compressed_positionlist_buffer.end(), one_doc_positions,
            one_doc_positions + one_doc_positions_len);
        position_lens.push_back(one_doc_positions_len);
        delete[] one_doc_positions;
      }

      // compress position lens
      unit_reserve_size = 1024;

      if (unit_docid_num == 128) {
        bc_position_length.compress(position_lens.data(), unit_docid_num, unit,
                                    unit_reserve_size);

      } else {
        vbc_no_delta.compress(position_lens.data(), unit_docid_num, unit,
                              unit_reserve_size);
        last_unit_positions_compress_size = unit_reserve_size;
      }
      compressed_postinglist_buffer.insert(compressed_postinglist_buffer.end(),
                                           unit, unit + unit_reserve_size);
    }

    /*invert index format:
     *|4bytes |4bytes |8byes                |8bytes|4bytes                | | |
     *| |max doc|doc num|posting_list_offset|position_list_offset|jump table
     *entry count|jump entry data|posting data|position data| posting data
     *format: |compressed posting data(128 docs per unit)|position lens|
     */
    MemoryFile buffer;
    buffer.append(&max_doc, sizeof(max_doc));
    buffer.append(&doc_num, sizeof(doc_num));
    buffer.append(&last_unit_docids_compress_size,
                  sizeof(last_unit_docids_compress_size));
    buffer.append(&last_unit_positions_compress_size,
                  sizeof(last_unit_positions_compress_size));
    uint32_t jump_table_entry_count = jump_table.size();
    uint64_t posting_list_offset =
        jump_table_entry_count * sizeof(JumptTableEntry);
    uint64_t position_list_offset =
        jump_table_entry_count * sizeof(JumptTableEntry) +
        compressed_postinglist_buffer.size();
    buffer.append(&posting_list_offset, sizeof(uint64_t));
    buffer.append(&position_list_offset, sizeof(uint64_t));
    buffer.append(&jump_table_entry_count, 4);
    buffer.append(jump_table.data(),
                  jump_table_entry_count * sizeof(JumptTableEntry));
    buffer.append(compressed_postinglist_buffer.data(),
                  compressed_postinglist_buffer.size());
    buffer.append(compressed_positionlist_buffer.data(),
                  compressed_positionlist_buffer.size());
    db->set(key, buffer.content(), buffer.size());
  }
  delete[] unit;
  db->close();
}

void InvertFieldsIndexWriter::add(uint32_t docid,
                                  std::shared_ptr<Field> field) {
  TextField* tf = dynamic_cast<TextField*>(field.get());
  auto ti = tokenizer_->get_term_iterator(tf->get_value());
  while (ti->next()) {
    Term term = ti->term();
    term.set_field(tf->get_name());
    int position = ti->position();
    std::string key = term.get_term() + term.get_field();
    if (position_lists_.count(key) == 0) {
      std::vector<uint32_t> posting_list;
      posting_list.push_back(docid);
      posting_lists_[key] = posting_list;

      std::vector<std::vector<uint32_t>> position_lists;
      std::vector<uint32_t> position_list;
      position_list.push_back(position);
      position_lists.push_back(position_list);
      position_lists_[key] = position_lists;

    } else {
      uint32_t last = *(posting_lists_[key].rbegin());
      if (docid == last) {
        position_lists_[key].rbegin()->push_back(position);
      } else {
        posting_lists_[key].push_back(docid);
        std::vector<uint32_t> position_list;
        position_list.push_back(position);
        position_lists_[key].push_back(position_list);
      }
    }
  }
}

TermReader* InvertFieldsIndexWriter::get_reader(Term* term) {
  std::string key = term->get_term() + term->get_field();
  if (posting_lists_.count(key) == 0) {
    return nullptr;
  } else
    return new MemoryTermReader(posting_lists_[key], position_lists_[key]);
}

int InvertFieldsIndexWriter::open() { return 0; }

int InvertFieldsIndexWriter::close() { return 0; }
}  // namespace yas
