#include <pthread.h>

#pragma once

namespace yas {
class SharedMutex {
 private:
  /* data */
  pthread_rwlock_t rwlock_;

 public:
  SharedMutex(/* args */);
  ~SharedMutex();
  SharedMutex(const SharedMutex&) = delete;
  SharedMutex& operator=(const SharedMutex&) = delete;
  void lock();
  void unlock();
  void lock_shared();
  void unlock_shared();
};

}  // namespace yas