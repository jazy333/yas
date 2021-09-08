#pragma once
#include <pthread.h>
#include <string>

#include "file.h"

namespace yas {
class MMapFile : public File {
 public:
  MMapFile();
  ~MMapFile();
  int open(const std::string& path, bool writable,bool trunc=false) override;
  int close() override;
  int read(int64_t off, void* buf, size_t size) override;
  int read(void* buf, size_t size) override;
  int write(int64_t off, const void* buf, size_t size) override;
  int append(const void* buf, size_t size, int64_t* off = nullptr) override;
  int sync() override;
  int truncate(size_t size) override;
  int size(int64_t* size) override;
  size_t size() override;
  off64_t seek(off64_t offset) override;
  std::unique_ptr<File> make() override;

 private:
  int adjust(bool writable, int64_t off, size_t size);
  int32_t fd_;
  int64_t file_size_;
  char* map_;
  int64_t map_size_;
  int64_t lock_size_;
  bool writable_;
  int32_t open_options_;
  pthread_rwlock_t rwlock_;
  off_t offset_;
  std::string path_;
};
}  // namespace yas
