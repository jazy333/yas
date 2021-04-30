#pragma once
#include <sys/types.h>
namespace yas {
ssize_t pread_with_check(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite_with_check(int fd, const void *buf, size_t count, off_t offset);
ssize_t read_with_check(int fd, void *buf, size_t count);
ssize_t write_with_check(int fd, const void *buf, size_t count);
inline int round(int len, int align) { return ((len - 1) / align + 1) * align; }
int gcd(int a,int b);
int lcm(int a,int b);

}  // namespace yas