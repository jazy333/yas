#include "invert_field_index_writer.h"

#include <iterator>

#include "memory_file.h"
#include "memory_invert_field_index_reader.h"
#include "simd_binary_compression.h"
namespace yas {
InvertFieldIndexWriter::InvertFieldIndexWriter(/* args */);
InvertFieldIndexWriter::~InvertFieldIndexWriter();
void InvertFieldIndexWriter::flush(DB* db) {
  for (auto&& item : posting_lists_) {
    std::string key = item.first();
    auto& docids = item.second();
    auto& postions = position_lists_[key];
    uint32_t max_doc = docids.rbegin();
    size_t doc_num = docids.size();
    SIMDBinaryCompression<true> bc_posting_list;
    SIMDBinaryCompression<false> bc_position_length;
    std::vector<uint8_t> compressed_postinglist_buffer;
    std::vector<uint8_t> compressed_positionlist_buffer;
    std::vector<uint32_t> jump_tables;
    size_t unit_reserve_size = 1024;
    uint8_t* unit = new uint8_t[unit_reserve_size];
    for (int i = 0; i < doc_num; i += 128) {
      unit_reserve_size = 1024;

      bc_posting_list.compress(docids.data() + i, 128, unit,
                               &unit_reserve_size);
      size_t posting_list_offset = compressed_postinglist_buffer.size();
      jump_tables.push_back(docids[i + 127]);
      jump_tables.push_back(posting_list_offset);
      compressed_postinglist_buffer.insert(compressed_postinglist_buffer.end(),
                                           std::begin(unit),
                                           std::end(unit + unit_reserve_size));
      bc_posting_list.set_init(docids[i]);

      posting_list_offset = compressed_positionlist_buffer.size();
      jump_tables.push_back(posting_list_offset);
      VariableByteCompression<true> vbc;
      std::vector<uint32_t> position_lens;
      for (int j = i; j < i + 128; ++j) {
        size_t one_doc_positions_len = postions[j].size() * 2 * 4;
        uint8_t* one_doc_positions = new uint8_t[one_doc_positions_len];
        vbc.compress(postions[j].data(), postions[j].size(), one_doc_positions,
                     one_doc_positions_len);
        position_lens.push_back(one_doc_positions_len);
        compressed_positionlist_buffer.insert(
            compressed_positionlist_buffer.end(), std::begin(one_doc_positions),
            std::end(one_doc_positions + one_doc_positions_len));
        position_lens.push_back(one_doc_positions_len);
        delete[] one_doc_positions;
      }

      bc_position_length.compress(position_lens, 128, unit, unit_reserve_size);
      compressed_postinglist_buffer.insert(compressed_postinglist_buffer.end(),
                                           std::begin(unit),
                                           std::end(unit + unit_reserve_size));
    }

    MemoryFile buffer;
    buffer.write(&max_doc, sizof(max_doc));
    buffer.write(&doc_num, sizeof(doc_num));
    uint32_t posting_list_offset = buffer.size() + jumm_table.size() * 4;
    uint32_t positions_list_offset = buffer.size() + jumm_table.size() * 4 +
                                     compressed_postinglist_buffer.size();
    buffer.write(&posting_list_offset, sizeof());
    uint32_t jump_table_entry_count = jump_table.size();
    buffer.write(&jump_table_entry_count, 4);
    buffer.write(jump_table.data(), jump_table_entry_count * 4);
    buffer.write(compressed_postinglist_buffer.data(),
                 compressed_postinglist_buffer.size());
    buffer.write(compressed_positionlist_buffer.data(),
                 compressed_positionlist_buffer.size());
    db->set(key, buffer.content(), buffer.size());
    delete unit;
  }
}

void InvertFieldIndexWriter::add(uin32_t docid, Field* field) {
  TextField* tf = dynamic_cast<TextField*>(field);
  std::vector<int> postion_list;
  TermIterator* ti = tokenizer_->get_term_iterator(tf->get_value());
  while (ti->next()) {
    Term term = ti->term();
    term.set_field(tf->get_field());
    int postion = ti->postion();
    std::string key = term->get_term() + term->get_field();
    if (position_lists_.count(key) == 0) {
      std::vector<uint32_t> posting_list;
      posting_list.push_back(docid);
      std::vector<std::vector<int>> postion_lists;
      std::vector<int> position_list;
      position_list.push_back(postion);
      postion_lists.push_back(postion_list);
      position_lists_[key] = position_list;

    } else {
      if (docid == posting_lists_[key].rbegin()) {
        position_lists_[key].rbegin()->push_back(postion);
      } else {
        posting_lists_[key].push_back(docid);
        std::vector<int> position_list;
        position_list.push_back(postion);
        postion_lists.push_back(postion_list);
        position_lists_[key] = position_list;
      }
    }

    position_list.push_back(position);
  }
}

FieldIndexReader* InvertFieldIndexWriter::get_reader(Term term) {
  std::string key = term.get_term() + term.get_field();
  if (posting_lists_.count(key) == 0)
    return nullptr;
  else
    return new MemoryInvertFieldIndexReader(&posting_lists_[key],
                                            &position_lists_[key]);
}
}  // namespace yas
