#pragma once

namespace yas {
template <tyepname mutex>
class SharedLock {
 public:
  SharedLock() : mutex_(nullptr) {}
  SharedLock(mutex& mutex) : mutex_(&mutex) { mutex_->lock_shared(); }
  SharedLock(SharedLock&& lock)
      : SharedLock(){swap(lock)} SharedLock(const SharedLock&) = delete;
  SharedLock& operator=(const SharedLock&) = delete;

  SharedLock& operator=(SharedLock&& lock) {
    SharedLock(std::move(lock)).swap(*this);
    return *this;
  }

  ~SharedLock() { mutex_->unlock_shared() }

 private:
  mutex* mutex_;
  void swap(SharedLock& lock) { swap(mutex_, lock.mutex_); }
};
}  // namespace yas