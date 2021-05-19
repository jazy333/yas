#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
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
  virtual int read(void* buf, size_t size) = 0;
  virtual int read(std::string& buf, size_t size) {
    buf.resize(size, 0);
    return read(const_cast<char*>(buf.data()), size);
  }
  virtual int write(int64_t off, const void* buf, size_t size) = 0;
  virtual int write(int64_t off, const std::string& buf) {
    return write(off, buf.data(), buf.size());
  }

  template <class Type>
  int read_vint(loff_t off, Type& val) {
    static_assert(
        (std::is_same<Type, long>::value || std::is_same<Type, int>::value ||
         std::is_same<Type, short>::value),
        "variable length byte read only support:long,int,short");
    char one;
    size_t max = sizeof(Type) + 1;
    size_t count = 0;
    val = 0;
    while (count < max) {
      int ret = read(off + count, &one, 1);
      if (ret == 1) {
        val |= (one & 0x7f) << (count * 7);
        if (!(one & 0x80)) break;
      } else {
        return ret;
      }
      ++count;
    }
    return count+1;
  }

  template <class Type>
  int write_vint(Type val) {
    static_assert(
        (std::is_same<Type, long>::value || std::is_same<Type, int>::value ||
         std::is_same<Type, short>::value),
        "variable length byte write only support:long,int,short");
    char output[sizeof(Type) + 1];
    int i = 0;
    for (;; ++i) {
      if (!(val & ~0x7f)) {
        output[i++] = (val & 0x7f);
        break;
      } else
        output[i] = (val & 0x7f) | 0x80;
      val = val >> 7;
    }
    return append(output, i);
  }
  virtual int append(const void* buf, size_t size, int64_t* off = nullptr) = 0;
  virtual int append(const std::string& buf, int64_t* off = nullptr) {
    return append(buf.data(), buf.size(), off);
  }
  virtual int truncate(size_t size) = 0;
  virtual int sync() { return 0; }
  virtual int size(int64_t* size) = 0;
  virtual size_t size() = 0;
  virtual off64_t seek(off64_t offset) = 0;
  virtual std::unique_ptr<File> make() = 0;
};
}  // namespace yas