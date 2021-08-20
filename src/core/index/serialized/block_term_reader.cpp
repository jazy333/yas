#include "block_term_reader.h"

#include <algorithm>

#include "algorithm.h"
#include "bm25_relevance.h"
#include "simd_binary_compression.h"
#include "variable_byte_compression.h"

namespace yas {
BlockTermReader::BlockTermReader(DB* db, Term* term)
    : db_(db),
      term_(term),
      docid_(0),
      current_jump_table_entry_index_(0),
      entries_(nullptr),
      current_entry_(nullptr),
      current_unit_index_(128),
      current_unit_max_docid_(0),
      unit_size_(0) {
  current_unit_docids_.resize(128, 0);
}

uint32_t BlockTermReader::next() {
  current_unit_position_index_ = 0;
  if (current_unit_index_ + 1 < unit_size_) {
    current_unit_index_++;
    docid_ = current_unit_docids_[current_unit_index_];
    return docid();
  }
  return advance(current_unit_max_docid_ + 1);
}

int BlockTermReader::read_data() {
  std::string key = term_->get_term() + term_->get_field();
  int ret = db_->get(key, invert_index_);
  if (ret < 0 || invert_index_.size() == 0) return ret;

  meta_ = (InvertIndexMeta*)(invert_index_.data());
  entries_ = (JumpTableEntry*)(invert_index_.data() + sizeof(InvertIndexMeta));
  return 0;
}

void BlockTermReader::reset_current_unit() {
  current_unit_index_ = 128;
  current_unit_position_index_ = 0;
  current_unit_docids_.clear();
  current_unit_docids_.resize(128, 0);
  current_unit_positions_.clear();
}

void BlockTermReader::next_unit(uint32_t target) {
  // first init
  if (!entries_) {
    read_data();
  }

  // not not entry,return
  if (!entries_) {
    return;
  }

  reset_current_unit();
  auto last = entries_ + meta_->jump_table_entry_count;
  current_entry_ =
      exponential_search(entries_ + current_jump_table_entry_index_, last,
                         target, compare_with_jump_table_entry());

  SIMDBinaryCompression<true> sbc;
  SIMDBinaryCompression<false> sbc_position_lens;
  VariableByteCompression<true> vbc;
  VariableByteCompression<false> vbc_no_delta;
  if (current_entry_ != last) {
    bool last_unit = false;
    if (current_entry_ == last - 1 && num_docs_ % 128 != 0) {
      last_unit = true;
    }

    current_jump_table_entry_index_ = current_entry_ - entries_;
    current_unit_max_docid_ = current_entry_->max_docid;
    uint8_t* position_len_start = nullptr;
    size_t in_size=0;
    size_t out_size = current_unit_docids_.size() * sizeof(uint32_t);
    if (!last_unit) {
      position_len_start = sbc.decompress(
          reinterpret_cast<const uint8_t*>(invert_index_.data()) +
              current_entry_->posting_list_offset,
          in_size, current_unit_docids_.data(), out_size);
      sbc.set_init(*(current_unit_docids_.rbegin()));
    } else {
      in_size = meta_->last_unit_posting_list_compress_size;
      position_len_start = vbc.decompress(
          reinterpret_cast<const uint8_t*>(invert_index_.data()) +
              current_entry_->posting_list_offset,
          in_size, current_unit_docids_.data(), out_size);
    }
    unit_size_ = out_size;
    std::vector<uint32_t> position_lens(128);
    size_t position_lens_len = 4 * 128;

    if (!last_unit)
      sbc_position_lens.decompress(position_len_start, in_size,
                                   position_lens.data(), position_lens_len);
    else {
      in_size = meta_->last_unit_position_compress_size;
      vbc_no_delta.decompress(position_len_start, in_size, position_lens.data(),
                              position_lens_len);
    }
    uint64_t position_data_offset =
        meta_->position_list_start + current_entry_->position_list_offset;
    uint8_t* position_data_start =
        reinterpret_cast<uint8_t*>(const_cast<char*>(invert_index_.data())) +
        position_data_offset;
    for (int i = 0; i < unit_size_; ++i) {
      size_t len = position_lens[i];
      size_t freq = len;
      std::vector<uint32_t> positions(len);
      position_data_start =
          vbc.decompress(position_data_start, len, positions.data(), freq);
      current_unit_positions_.push_back(std::move(positions));
    }
  } else {
    docid_ = NDOCID;
  }
}

uint32_t BlockTermReader::advance(uint32_t target) {
  current_unit_position_index_ = 0;
  if (target > max_doc_) return NDOCID;

  if (target > current_unit_max_docid_) {
    next_unit(target);
  }

  auto iter = std::lower_bound(current_unit_docids_.begin(),
                               current_unit_docids_.end(), target);
  if (iter != current_unit_docids_.end()) {
    current_unit_index_ = std::distance(iter, current_unit_docids_.begin());
    docid_ = current_unit_docids_[current_unit_index_];
    return docid_;
  } else {
    return NDOCID;
  }
}

uint32_t BlockTermReader::docid() { return docid_; }

long BlockTermReader::cost() { return num_docs_; }

std::string BlockTermReader::name() { return "BlockTermReader"; }

float BlockTermReader::score() { return 0.0f; }

int BlockTermReader::freq() {
  return current_unit_positions_[current_unit_index_].size();
}

int BlockTermReader::next_postion() {
  return current_unit_positions_[current_unit_index_]
                                [current_unit_position_index_++];
}

}  // namespace yas