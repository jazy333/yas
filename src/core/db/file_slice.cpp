#include "file_slice.h"

namespace {
FileSlice::FileSlice(File* file, loff_t off, loff_t length)
    : file_(file), off_(off), length_(length) {}

int FileSlice::read(void* buf, size_t size) {
  int ret = file_->read(off_, buf, size);
  off_ += size;
  return ret;
}

int FileSlice::read(loff_t off, void* buf, size_t size) {
  return file_->read(off, buf, size);
}

void FileSlice::seek(loff_t off) { off_ = off; }
}  // namespace