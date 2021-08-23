#include "file_slice.h"

namespace yas {
FileSlice::FileSlice(File* file, loff_t off, loff_t length)
    : file_(file), off_(0), original_off_(off), length_(length) {}

int FileSlice::read(void* buf, size_t size) {
  int ret = file_->read(original_off_+off_, buf, size);
  off_ += size;
  return ret;
}

int FileSlice::read(loff_t off, void* buf, size_t size) {
  return file_->read(original_off_ + off, buf, size);
}

void FileSlice::seek(loff_t off) { off_ = off; }

uint64_t FileSlice::seek() { return off_; }
}  // namespace yas