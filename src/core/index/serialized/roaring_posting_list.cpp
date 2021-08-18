#include "roaring_posting_list.h"

#include "file_slice.h"
#include "fixed_bit_set.h"

namespace yas {
RoaringPostingList::RoaringPostingList(File* in, uint64_t off, uint64_t length,
                                       int jump_table_entry_count, long cost)
    : docid_(0),
      cost_(cost),
      jump_table_entry_count_(jump_table_entry_count),
      in_(in) {
  size_t jump_table_length = jump_table_entry_count * 2 * sizeof(int);
  loff_t jump_off = length - jump_table_length;
  jump_table_slice_ = std::unique_ptr<FileSlice>(
      new FileSlice(in, jump_off, jump_table_length));
  block_slice_ = std::unique_ptr<FileSlice>(new FileSlice(in, off, length));
}

RoaringPostingList::~RoaringPostingList() {}

uint32_t RoaringPostingList::next() { return advance(docid_ + 1); }

bool RoaringPostingList::advance_exact_in_block(uint32_t target) {
  uint16_t target_in = target & 0x0000ffff;
  switch (type_) {
    case 0: {
      uint16_t doc;
      for (; index_ < next_index; ++index_) {
        block_slice_->read(doc);
        if (doc >= target_in) {
          docid_ = block_ << 16 | doc;
          return true;
        }
      }
      return false;
    }
    case 1: {
      docid_ = target;
      index_ += target_in;
      return true;
    }
    case 2: {
      int bit_index = target_in >> 6;
      int word_index = target_in % 64;
      uint64_t word;
      uint16_t min;
      block_slice->read(min);
      block_slice->read(bit_index * sizeof(uint64_t), word);
      word = word >> word_index;
      if (word & 1) {
        docid_ = target;
        return true;
      } else {
        return false;
      }
    }
    default:
      break;
  }
}

void RoaringPostingList::read_block_header() {
  uint16_t block;
  block_slice_->read(block);
  uint16_t cardinality;
  block_slice_->read(cardinality);
  int num = cardinality + 1;
  if (num < 4096) {
    type_ = 0;
    block_end_ = block_slice_->seek() + num << 1;  // uint16,size==2
  } else if (num == 65536) {
    type_ = 1;
    block_end_ = block_slice->seek();
  } else {
    type_ = 2;
    block_end_ = block_slice_->seek() + (num + 1) * 8 / 64;
  }
}

int RoaringPostingList::advance_block(uint32_t target) {
  int block = target >> 16;
  if (block > jump_table_entry_count_) block = jump_table_entry_count_ - 1;
  int offset;
  jump_table_slice_->read(block * 2 * sizeof(int), index_);
  jump_table_slice_->read(block * 2 * sizeof(int), offset);
  block_slice_->seek(offset);
  read_block_header();
}

uint32_t RoaringPostingList::advance(uint32_t target) {
  uint16_t block = target >> 16 & 0xffff;
  if (block_ < block) {
    advance_block(block);
  }

  if (block == block_) {
    bool found = advance_exact_in_block(target);
  }

  return docid_;
}

bool RoaringPostingList::advance_exact(uint32_t target) {
  uint16_t target_block = target >> 16& 0xffff;
  if (block_ < target_block) {
    advance_block(target);
  }
  if (block_ == target_block && advance_exact_in_block(target))
    return true;
  else
    return false;
}

uint32_t RoaringPostingList::docid() { return docid_; }

long RoaringPostingList::cost() { return cost_; }

std::string RoaringPostingList::name() { return "RoaringPostingList"; }

int RoaringPostingList::index() { return index_; }

void RoaringPostingList::add_jumps(std::vector<int>& jump_tables, int from,
                                   int to, int cardinality, uint64_t offset) {
  for (; from < to; ++from) {
    jump_tables.push_back({cardinality, static_cast<uint32_t>(offset)});
  }
}

uint16_t RoaringPostingList::flush_jumps(File* out,
                                         std::vector<int>& jump_tables) {
  out.write(jump_tables.data(), jump_tables.size() * sizeof(int));
  return static_cast<uint16_t>(jump_tables.size());
}

void RoaringPostingList::flush(File* out, std::vector<uint32_t>& docids,
                               int from, int to, int cardinality,
                               uint16_t block) {
  out.write(&block, 2);  // block num
  uint16_t cardi = static_cast<uint16_t>(cardinality - 1);
  out.write(&cardi, 2);  // cardinality-1

  if (cardi > 4095) {  // dense
    if (cardi != 66535) {
      uint16_t min = (docids[from] >> 16) & 0xffff;
      uint16_t max = (docids[to - 1] >> 16) & 0xffff;
      uint16_t bit_num = max - min;
      FixedBitSet<bit_num> bitset;
      out.append(&min, sizeof(min));

      for (; from < to; ++from) {
        bitset.set((docids[from] >> 16) & 0xffff - min);
      }
      uint64_t* bits = bitset.bits();
      uint16_t cap = static_cast<uint16_t>(bitset.capcity());
      out.append(bits, cap * sizeof(uint64_t));
    }  // full,do nothing

  } else {
    out.append(docids.data() + i, (to - from) * sizeof(uint32_t));
  }
}

uint16_t RoaringPostingList::flush(File* out, std::vector<uint32_t>& docids) {
  size_t start_offset = out->size();
  uint64_t total_cardinality = 0;
  uint64_t block_cardinality = 0;
  int block = 0;
  int pre_block = -1;
  int start_index = 0;
  int jump_block = 0;
  std::vector<int> jump_tables;
  for (int i = 0; i < docids.size(); ++i) {
    block = docids[i] >> 16;
    if (pre_block != -1 && block != pre_block) {
      uint64_t block_offset = out.size() - start_offset;
      add_jumps(jump_tables, jump_block, pre_block + 1, total_cardinality,
                block_offset);
      flush(out, docids, start_index, i, block_cardinality, pre_block);
      total_cardinality += block_cardinality;
      block_cardinality = 0;
      start_index = i;
      jump_block = pre_block + 1;
    }

    pre_block = block;
    block_cardinality++;
  }

  if (block_cardinality > 0) {
    uint64_t block_offset = out.size() - start_offset;
    add_jumps(jump_tables, jump_block, pre_block + 1, total_cardinality,
              block_offset);
    flush(out, docids, start_index, i, block_cardinality, pre_block);
    total_cardinality += block_cardinality;
    pre_block++;
  }

  return flush_jumps(out, jump_tables);
}

}  // namespace yas