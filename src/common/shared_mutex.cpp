#include "shared_mutex.h"

namespace yas {
SharedMutex::SharedMutex(/* args */) { pthread_rwlock_init(&rwlock_, nullptr); }

SharedMutex::~SharedMutex() {}

void SharedMutex::lock() { pthread_rwlock_wrlock(&rwlock_); }
void SharedMutex::unlock() { pthread_rwlock_unlock(&rwlock_); }
void SharedMutex::lock_shared() { pthread_rwlock_rdlock(&rwlock_); }
void SharedMutex::unlock_shared() { pthread_rwlock_unlock(&rwlock_); }

}  // namespace yas