
#define _LARGEFILE64_SOURCE
#include "mmap_file.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>

#include "common/common.h"
#include "common/const.h"

namespace yas {

MMapFile::MMapFile()
    : fd_(-1),
      file_size_(0),
      map_(nullptr),
      map_size_(1 << 20LL),
      lock_size_(0),
      writable_(true),
      open_options_(0),
      offset_(0) {
  pthread_rwlock_init(&rwlock_, nullptr);
}

MMapFile::~MMapFile() {
  if (fd_ > 0) {
    close();
  }
}

int MMapFile::open(const std::string& path, bool writable) {
  if (fd_ > 0) {
    return -1;
  }

  int32_t oflags = O_RDONLY;

  if (writable) {
    oflags = O_RDWR | O_CREAT;
  }

  int32_t fd = ::open(path.c_str(), oflags, 0644);
  if (fd < 0) {
    std::cout << "open error:" << std::strerror(errno) << std::endl;
    return -1;
  }

  struct stat st;
  if (fstat(fd, &st) < 0) {
    ::close(fd);
    std::cout << "fstat error:" << std::strerror(errno) << std::endl;
    return -1;
  }
  int64_t file_size = st.st_size;

  int64_t map_size = map_size_;
  int32_t mprot = PROT_READ;

  void* map = nullptr;
  if (map_size_ > 0) {
    if (writable) {
      const int64_t diff = map_size % PAGE_SIZE;
      if (diff > 0) {
        map_size += PAGE_SIZE - diff;
      }
      mprot |= PROT_WRITE;

    } else {
      map_size = std::max(map_size, static_cast<int64_t>(PAGE_SIZE));
    }
    map = mmap(0, map_size, mprot, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
      std::cout << "map error:" << std::strerror(errno) << std::endl;
      ::close(fd);
      return -1;
    }
  }

  fd_ = fd;
  file_size_ = file_size;
  map_size_ = map_size;
  map_ = static_cast<char*>(map);
  writable_ = writable;
  return 0;
}

int MMapFile::close() {
  int ret = 0;
  if (map_size_ > 0) {
    const int64_t unmap_size =
        std::max(map_size_, static_cast<int64_t>(PAGE_SIZE));
    if (munmap(map_, unmap_size) != 0) {
      ret = -1;
    }
  }

  ::close(fd_);
  pthread_rwlock_destroy(&rwlock_);

  fd_ = -1;
  file_size_ = 0;
  map_ = nullptr;
  map_size_ = 0;
  writable_ = false;

  return ret;
}

int MMapFile::adjust(bool writable, int64_t off, size_t size) {
  if (off < 0) {
    off = file_size_;
  }
  const int64_t end_position = off + size;
  if (end_position > map_size_) {
    int64_t new_map_size =
        std::max(std::max(end_position, static_cast<int64_t>(map_size_)),
                 static_cast<int64_t>(PAGE_SIZE));
    const int64_t diff = new_map_size % PAGE_SIZE;
    if (diff > 0) {
      new_map_size += PAGE_SIZE - diff;
    }
    if (ftruncate(fd_, new_map_size) != 0) {
      return -1;
    }
    void* new_map = mremap(map_, map_size_, new_map_size, MREMAP_MAYMOVE);
    if (new_map == MAP_FAILED) {
      return -1;
    }
    map_ = static_cast<char*>(new_map);
    map_size_ = new_map_size;
  }
  file_size_ = std::max(file_size_, end_position);
  return 0;
}

int MMapFile::read(int64_t off, void* buf, size_t size) {
  pthread_rwlock_rdlock(&rwlock_);
  int64_t read_end = off + size;
  if (read_end > file_size_) {
    std::cout << "read exceed end:" << std::endl;
    pthread_rwlock_unlock(&rwlock_);
    return -1;
  }

  if (read_end > map_size_) {
    pread_with_check(fd_, buf, size, off);
  } else {
    std::memcpy(buf, map_ + off, size);
  }
  pthread_rwlock_unlock(&rwlock_);
  return size;
}

int MMapFile::read(void* buf, size_t size) {
  pthread_rwlock_rdlock(&rwlock_);
  int64_t read_end = offset_ + size;
  if (read_end >= file_size_) {
    std::cout << "read exceed end:" << std::endl;
    return -1;
    pthread_rwlock_unlock(&rwlock_);
  }

  if (read_end > map_size_) {
    pread_with_check(fd_, buf, size, offset_);
  } else {
    std::memcpy(buf, map_ + offset_, size);
  }
  offset_ += size;
  pthread_rwlock_unlock(&rwlock_);
  return size;
}

int MMapFile::write(int64_t off, const void* buf, size_t size) {
  pthread_rwlock_wrlock(&rwlock_);
  int64_t new_off = off;
  if (off < 0) {
    new_off = file_size_;
  }
  int64_t write_end = new_off + size;
  if (write_end > file_size_ || write_end > map_size_) {
    ssize_t ret = pwrite_with_check(fd_, buf, size, new_off);
    if (off < 0)
      file_size_ += ret;
    else {
      if (ret + new_off > file_size_) {
        file_size_ = ret + new_off;
      }
    }

  } else
    std::memcpy(map_ + off, buf, size);
  pthread_rwlock_unlock(&rwlock_);
  return size;
}

int MMapFile::append(const void* buf, size_t size, int64_t* off) {
  if (off) {
    *off = file_size_;
  }
  write(-1, buf, size);

  return size;
}

int MMapFile::truncate(size_t size) {
  if (!writable_) return -1;
  pthread_rwlock_wrlock(&rwlock_);
  if (ftruncate(fd_, size) != 0) {
    return -1;
    pthread_rwlock_unlock(&rwlock_);
  }
  file_size_ = size;
  pthread_rwlock_unlock(&rwlock_);
  return 0;
}

int MMapFile::sync() {
  pthread_rwlock_wrlock(&rwlock_);
  int status = 0;
  if (fd_ < 0) return -1;
  if (!writable_) return -1;

  if (msync(map_, map_size_, MS_SYNC) != 0) {
    status = -1;
  }
  if (fsync(fd_) != 0) {
    status = -1;
  }
  pthread_rwlock_unlock(&rwlock_);
  return status;
}

int MMapFile::size(int64_t* size) {
  *size = file_size_;
  return 0;
}

off64_t MMapFile::seek(off64_t offset) {
  return lseek64(fd_, offset, SEEK_SET);
}

size_t MMapFile::size() { return file_size_; }

std::unique_ptr<File> MMapFile::make() {
  return std::unique_ptr<File>(new MMapFile());
}
}  // namespace yas