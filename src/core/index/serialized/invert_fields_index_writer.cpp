#include "invert_fields_index_writer.h"

#include <iterator>
#include <memory>

#include "memory_file.h"
#include "memory_term_reader.h"
#include "simd_binary_compression.h"
#include "text_field.h"
#include "variable_byte_compression.h"
#include "core/log/log.h"
#include "core/db/hash_db.h"

namespace yas {
InvertFieldsIndexWriter::InvertFieldsIndexWriter(IndexStat* index_stat)
    : tokenizer_(std::shared_ptr<Tokenizer>(new SimpleTokenizer(2))),
      index_stat_(index_stat) {}

InvertFieldsIndexWriter::InvertFieldsIndexWriter(IndexStat* index_stat,
    std::shared_ptr<Tokenizer> tokenizer)
    : tokenizer_(tokenizer),index_stat_(index_stat) {}

InvertFieldsIndexWriter::~InvertFieldsIndexWriter() {}

void InvertFieldsIndexWriter::flush(
    std::shared_ptr<DB> db, std::string key, std::vector<uint32_t>& docids,
    std::vector<std::vector<uint32_t>>& positions) {
  size_t unit_reserve_size = 2000;
  auto unit_ptr = std::unique_ptr<uint8_t[]>(new uint8_t[unit_reserve_size]());
  uint8_t* unit = unit_ptr.get();
  uint32_t max_doc = *(docids.rbegin());
  size_t doc_num = docids.size();
  SIMDBinaryCompression<true> bc_posting_list;
  SIMDBinaryCompression<false> bc_position_length;
  VariableByteCompression<true> vbc_delta;
  VariableByteCompression<false> vbc_no_delta;
  std::vector<uint8_t> compressed_postinglist_buffer;
  std::vector<uint8_t> compressed_positionlist_buffer;
  std::vector<JumptTableEntry> jump_table;
  uint32_t last_unit_docids_compress_size = 0;
  uint32_t last_unit_positions_compress_size = 0;
  int i = 0;
  size_t unit_docid_num = 0;
  for (int i = 0; i < doc_num; i += unit_docid_num) {
    JumptTableEntry entry;
    unit_reserve_size = 2000;
    if (i + 128 <= doc_num) {
      unit_docid_num = 128;
    } else {
      unit_docid_num = doc_num - i;
    }
    entry.max_docid = docids[i + unit_docid_num - 1];
    entry.posting_list_offset = compressed_postinglist_buffer.size();

    // compress docids
    if (unit_docid_num == 128) {
      bc_posting_list.compress(docids.data() + i, unit_docid_num, unit,
                               unit_reserve_size);

      bc_posting_list.set_init(entry.max_docid);
    } else {
      vbc_delta.compress(docids.data() + i, unit_docid_num, unit,
                         unit_reserve_size);
      last_unit_docids_compress_size = unit_reserve_size;
    }
    compressed_postinglist_buffer.insert(compressed_postinglist_buffer.end(),
                                         unit, unit + unit_reserve_size);

    // comress positions
    entry.position_list_offset =
        compressed_positionlist_buffer.size();  // positions list offset
    jump_table.push_back(entry);
    std::vector<uint32_t> position_lens;
    for (int j = i; j < i + unit_docid_num; ++j) {
      size_t one_doc_positions_len = positions[j].size() * 2 * sizeof(uint32_t);
      auto one_doc_positions_ptr =
          std::unique_ptr<uint8_t[]>(new uint8_t[one_doc_positions_len]());
      uint8_t* one_doc_positions = one_doc_positions_ptr.get();
      vbc_delta.compress(positions[j].data(), positions[j].size(),
                         one_doc_positions, one_doc_positions_len);
      position_lens.push_back(one_doc_positions_len);
#if 0
        for(int k=0;k<positions[j].size();++k){
          LOG_INFO("one_doc_positions[%d]:%d",i,one_doc_positions[k]);
        }
        LOG_INFO("one_doc_positions_len:%d,key=%s",one_doc_positions_len,key.c_str());

#endif
      compressed_positionlist_buffer.insert(
          compressed_positionlist_buffer.end(), one_doc_positions,
          one_doc_positions + one_doc_positions_len);

      // delete[] one_doc_positions;
    }

    // compress position lens
    unit_reserve_size = 2000;

    if (unit_docid_num == 128) {
      bc_position_length.compress(position_lens.data(), unit_docid_num, unit,
                                  unit_reserve_size);

    } else {
      vbc_no_delta.compress(position_lens.data(), unit_docid_num, unit,
                            unit_reserve_size);
#if 0
        for(int k=0;k<position_lens.size();++k){
          LOG_INFO("position_lens[%d]:%d",k,position_lens[k]);
        }
        LOG_INFO(
            "key=%s,position_lens.size=%d,unit_docid_num=%d,unit_reserve_size=%"
            "d",
            key.c_str(), position_lens.size(), unit_docid_num,
            unit_reserve_size);
#endif
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
  buffer.append(&posting_list_offset, sizeof(posting_list_offset));
  buffer.append(&position_list_offset, sizeof(position_list_offset));
  buffer.append(&jump_table_entry_count, sizeof(jump_table_entry_count));

  buffer.append(jump_table.data(),
                jump_table_entry_count * sizeof(JumptTableEntry));
  buffer.append(compressed_postinglist_buffer.data(),
                compressed_postinglist_buffer.size());
  buffer.append(compressed_positionlist_buffer.data(),
                compressed_positionlist_buffer.size());
  std::string value;
  value.append(buffer.content(), buffer.size());
  db->set(key, value);
  LOG_DEBUG(
      "key=%s,value "
      "size=%lu,max_doc=%u,doc_num=%d,last_unit_docids_compress_size=%u,last_"
      "unit_positions_compress_size=%u,jump_table_entry_count=%d",
      key.c_str(), buffer.size(), max_doc, doc_num,
      last_unit_docids_compress_size, last_unit_positions_compress_size,
      jump_table_entry_count);
  // delete[] unit;
}

void InvertFieldsIndexWriter::flush(FieldInfo fi, uint32_t max_doc,
                                    const IndexOption& option) {
  auto db = std::shared_ptr<HashDB>(new HashDB);
  std::string db_path = option.get_invert_file();
  db->open(db_path);

  for (auto&& item : posting_lists_) {
    std::string key = item.first;
    auto& docids = item.second;
    auto& positions = position_lists_[key];
    flush(db, key, docids, positions);
  }
  db->close();
}

int InvertFieldsIndexWriter::add(uint32_t docid, std::shared_ptr<Field> field) {
  TextField* tf = dynamic_cast<TextField*>(field.get());
  auto ti = tokenizer_->get_term_iterator(tf->get_value());
  int doc_len = 0;
  while (ti->next()) {
    index_stat_->total_term_freq++;
    doc_len++;
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
  return doc_len;
}

std::shared_ptr<TermReader> InvertFieldsIndexWriter::get_reader(Term term) {
  std::string key = term.get_term() + term.get_field();
  if (posting_lists_.count(key) == 0) {
    return nullptr;
  } else
    return std::shared_ptr<TermReader>(
        new MemoryTermReader(posting_lists_[key], position_lists_[key]));
}

int InvertFieldsIndexWriter::open() { return 0; }

int InvertFieldsIndexWriter::close() {
  posting_lists_.clear();
  position_lists_.clear();
  return 0;
}

DB* InvertFieldsIndexWriter::get_db() { return nullptr; }
}  // namespace yas
