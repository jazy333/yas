#include "memory_file.h"

#include <cstring>
#include <string>

namespace yas {
MemoryFile::MemoryFile() : writable_(true), open_(true), offset_(0) {}
MemoryFile::~MemoryFile() {}
int MemoryFile::open(const std::string& path, bool writable,bool trunc) {
  writable_ = writable;
  open_ = true;
  offset_ = 0;
  return 0;
}
int MemoryFile::close() {
  open_ = false;
  writable_ = false;
  return 0;
}
int MemoryFile::read(int64_t off, void* buf, size_t size) {
  if (!open_) return -1;
  if (off + size > data_.size()) {
    return -1;
  } else {
    memcpy(buf, data_.data() + off, size);
  }

  return size;
}
int MemoryFile::read(void* buf, size_t size) {
  int ret = read(offset_, buf, size);
  if (ret >= 0) {
    offset_ += ret;
  }
  return ret;
}
int MemoryFile::write(int64_t off, const void* buf, size_t size) {
  if (!open_ || !writable_) {
    return -1;
  } else {
    char* tmp = (char*)buf;
    data_.insert(data_.begin() + off, tmp, tmp + size);
  }
  return size;
}
int MemoryFile::append(const void* buf, size_t size, int64_t* off) {
  data_.insert(data_.end(), (char*)buf, (char*)buf + size);
  return size;
}

int MemoryFile::truncate(size_t size) {
  if (data_.size() > size) {
    data_.resize(size);
  }
  return 0;
}
int MemoryFile::size(int64_t* size) {
  *size = data_.size();
  return 0;
}
size_t MemoryFile::size() { return data_.size(); }

std::unique_ptr<File> MemoryFile::make() {
  return std::unique_ptr<File>(new MemoryFile());
}

int MemoryFile::sync() { return 0; }

off64_t MemoryFile::seek(off64_t offset) {
  offset_ = offset;
  return offset;
}

char* MemoryFile::content(){
  return data_.data();
}

}  // namespace yas