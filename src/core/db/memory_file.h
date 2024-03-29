#pragma once
#include <vector>

#include "file.h"

namespace yas {
class MemoryFile : public File {
 public:
  MemoryFile();
  ~MemoryFile();
  int open(const std::string& path, bool writable,bool trunc) override;
  int close() override;
  int read(int64_t off, void* buf, size_t size) override;
  int read(void* buf, size_t size) override;
  int write(int64_t off, const void* buf, size_t size) override;
  int append(const void* buf, size_t size, int64_t* off = nullptr) override;
  int sync() override;
  int truncate(size_t size) override;
  int size(int64_t* size) override;
  off64_t seek(off64_t offset) override;
  size_t size() override;
  std::unique_ptr<File> make() override;
  char* content();

 private:
  bool writable_;
  bool open_;
  off_t offset_;
  std::vector<char> data_;
};
}  // namespace yas