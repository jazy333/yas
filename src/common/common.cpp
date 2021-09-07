#include "common.h"

#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <utility>
#include <memory>

namespace yas {
ssize_t pread_with_check(int fd, void *buf, size_t count, off_t offset) {
  ssize_t ret;
  size_t io_bytes = 0;
  do {
    ret = pread(fd, (uint8_t *)(buf) + io_bytes, count - io_bytes,
                offset + io_bytes);
    if (ret > 0)
      io_bytes += ret;
    else if (ret == 0) {
      return io_bytes;
    } else if (ret < 0) {
      return io_bytes;
    }
  } while (io_bytes < count);
  return io_bytes;
}
ssize_t pwrite_with_check(int fd, const void *buf, size_t count, off_t offset) {
  ssize_t ret;
  size_t io_bytes = 0;
  do {
    ret = pwrite(fd, (const uint8_t *)(buf) + io_bytes, count - io_bytes,
                 offset + io_bytes);
    if (ret > 0)
      io_bytes += ret;
    else if (ret == 0) {
      errno = ENOSPC;
      return io_bytes;
    } else if (ret < 0) {
      return io_bytes;
    }
  } while (io_bytes < count);
  return io_bytes;
}

ssize_t read_with_check(int fd, void *buf, size_t count) {
  ssize_t ret;
  size_t io_bytes = 0;
  do {
    ret = read(fd, (uint8_t *)(buf) + io_bytes, count - io_bytes);
    if (ret > 0)
      io_bytes += ret;
    else if (ret == 0) {
      return io_bytes;
    } else if (ret < 0) {
      return io_bytes;
    }
  } while (io_bytes < count);
  return io_bytes;
}

ssize_t write_with_check(int fd, const void *buf, size_t count, off_t offset) {
  ssize_t ret;
  size_t io_bytes = 0;
  do {
    ret = write(fd, (const uint8_t *)(buf) + io_bytes, count - io_bytes);
    if (ret > 0)
      io_bytes += ret;
    else if (ret == 0) {
      errno = ENOSPC;
      return io_bytes;
    } else if (ret < 0) {
      return io_bytes;
    }
  } while (io_bytes < count);
  return io_bytes;
}

int lcm(int a, int b) { return (a * b) / gcd(a, b); }

uint32_t asmbits(const uint32_t v) {
  if (v == 0) return 0;
  uint32_t answer;
  __asm__("bsr %1, %0;" : "=r"(answer) : "r"(v));
  return answer + 1;
}

uint8_t uint2uchar(uint32_t value) {
  uint8_t bits = gccbits(value);
  if (bits < 4) return value;
  int shift = bits - 4;
  uint8_t fraction = value >> shift;
  uint8_t encode = ((shift + 1) << 3) | (fraction & 0x7);
  return encode;
}

uint32_t uchar2uint(uint8_t value) {
  int shift = (value >> 3) - 1;
  if (shift == -1) return value;

  int fraction = value & 0x07;
  uint32_t decode = (fraction | 0x80) << (shift);
  return decode;
}

bool string2wstring(const std::string& s,
                                     std::wstring& result) {
  setlocale(LC_ALL, "zh_CN.UTF-8");
  const char* source = s.c_str();
  size_t dst_size = s.size() + 1;
  auto dest_ptr = std::unique_ptr<wchar_t[]>(new wchar_t[dst_size]());
  auto dest = dest_ptr.get();
  size_t ret = mbstowcs(dest, source, dst_size);
  if (ret == -1) {
    return false;
  }
  result.assign(dest, ret);
  return true;
}

bool wstring2string(const std::wstring& s,
                                     std::string& result) {
  std::string cur_locale = setlocale(LC_ALL, NULL);

  const wchar_t* source = s.c_str();
  size_t dst_size = sizeof(wchar_t) * s.size() + 1;
  auto dest_ptr = std::unique_ptr<char[]>(new char[dst_size]());
  auto dest = dest_ptr.get();
  size_t ret = wcstombs(dest, source, dst_size);
  if (ret == -1) {
    return false;
  }
  result.assign(dest, ret);
  return true;
}

}  // namespace yas
