#pragma once
#include <iostream>
#include <memory>
#include <string>
namespace yas {
class File {
 public:
  virtual ~File() = default;
  virtual int open(const std::string& path, bool writable) = 0;
  virtual int close() = 0;
  virtual int read(int64_t off, void* buf, size_t size) = 0;
  virtual int read(int64_t off, std::string& buf, size_t size) {
    buf.resize(size, 0);
    return read(off, const_cast<char*>(buf.data()), size);
  }
  virtual int write(int64_t off, const void* buf, size_t size) = 0;
  virtual int write(int64_t off, const std::string& buf) {
    return write(off, buf.data(), buf.size());
  }
  virtual int append(const void* buf, size_t size, int64_t* off = nullptr) = 0;
  virtual int append(const std::string& buf, int64_t* off = nullptr) {
    return append(buf.data(), buf.size(), off);
  }
  virtual int truncate(size_t size) = 0;
  virtual int size(int64_t* size) = 0;
  virtual int64_t size() = 0;
  virtual std::unique_ptr<File> make() = 0;
};
}  // namespace yas