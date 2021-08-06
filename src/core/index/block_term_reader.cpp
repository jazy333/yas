#include "block_term_reader.h"

#include <algorithm>

#include "algorithm.h"
#include "bm25_relevance.h"
#include "simd_binary_compression.h"
#include "variable_byte_compression.h"
namespace yas {
BlockTermReader(DB* db, Term* term) db_(db), term_(term) {}

uint32_t BlockTermReader::next() {
  current_unit_position_index_ = 0;
  if (current_unit_index_++ < current_unit_docids_.size()) {
    return docid();
  }
  return advance(docid_ + 1);
}

void BlockTermReader::read_data() {
  std::string key = term_->get_term() + term_->get_field();
  db->get(key, value_);
  meta_ = (InvertIndexMeta*)(value_.data());
  entries = (JumpTableEntry*)(value_.data() + sizeof(InvertIndexMeta));
}

void BlockTermReader::reset_current_unit() {
  current_unit_max_docid_ = 0;
  current_unit_posting_list_offset_ = 0;
  current_unit_position_list_offset_ = 0;
  current_unit_index_ = 0;
  current_unit_position_index_ = 0;
  current_unit_docids_.clear();
  current_unit_positions_.clear();
}

bool BlockTermReader::compare_with_jump_table_entry(JumptTableEntry* a,
                                                    uint32_t target) {
  return a->max_docid < target;
}

void BlockTermReader::next_unit(uint32_t target) {
  reset_current_unit();
  auto last = entries + meta_->jump_table_entry_count;
  auto cur = exponential_search(entries + current_unit_, last, target,
                                compare_with_jump_table_entry);

  if (cur != last) {
    current_jump_table_entry_index_ = cur - entries;
    current_entry_ = cur;
    SIMDBinaryCompression<true> sbc;
    SIMDBinaryCompression<false> sbc_position_lens;
    VariableByteCompression<true> vbc;
    size_t in_size;
    uint8_t* position_len_start = sbc.decompress(
        invert_index_.data() + current_entry_->posting_list_offset, in_size,
        current_unit_docids_.data(), current_unit_docids_.size() * 4);

    std::vector<uint32_t> position_lens(128);
    size_t position_lens_len = 4 * 128;
    sbc_position_lens.decompress(static_cast<uint32_t*>(position_len_start),
                                 *position_len, position_lens,
                                 &position_lens_len);
    for (int i = 0; i < 128; ++i) {
      size_t len = position_lens[i];
      size_t freq = len;
      std::vector<uint32_t> positions(len);
      vbc.decompress(position_len_start, len, positions.data(), freq);
      positions.resize(freq);
      current_unit_positions_[i] = std::move(positions);
    }
  }
}

uint32_t BlockTermReader::advance(uint32_t target) {
  current_unit_position_index_ = 0;
  if (target > max_doc_) return NDOCID;

  if (target > current_unit_max_docid_) {
    next_unit();
  }

  auto iter = std::lower_bound(current_unit_docids_.begin(),
                               current_unit_docids_.end(), target);
  if (iter != current_unit_docids_.end()) {
    current_unit_index_ = std::distance(iter - current_unit_docids_.begin());
    return *iter;
  } else {
    return NDOCID;
  }
}

uint32_t BlockTermReader::docid() {
  return current_unit_docids_[current_unit_index_];
}

long BlockTermReader::cost() { return num_docs_; }

std::string BlockTermReader::name() { return "BlockTermReader"; }

float BlockTermReader::score() { return 0.0f; }

Scorer* BlockTermReader::scorer() {
  return new TermScorer(this, new BM25Relevance());
}

int BlockTermReader::freq() {
  return current_unit_position_list_offset_[current_unit_index_].size();
}

int BlockTermReader::next_postion() {
  return current_unit_position_list_offset_[current_unit_index_]
                                           [current_unit_postion_index_++];
}

}  // namespace yas