#pragma once
#include <type_traits>

#include "file.h"
namespace yas {
class FileSlice {
 public:
  FileSlice(File* file, loff_t off, loff_t length);
  template <class T>
  read(T& val) {
      static_assert(
        (std::is_integral<T>::value 
        "file slice read only support:integral");
    file_->read(off_, &val, sizeof(T));
    off_ += sizeof(T);
  }

  template <class T>
  read(loff_t off, T& val) {
      static_assert(
        (std::is_integral<T>::value 
        "file slice read only support:integral");
    file_->read(off, &val, sizeof(T));
  }
  int read(void* buf, size_t size);
  int read(loff_t off, void* buf, size_t size);
  void seek(loff_t off);

 private:
  File* file_;
  loff_t off_;
  loff_t length_;
};
}  // namespace yas