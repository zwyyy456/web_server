#include "locker.h"
#include <pthread.h>

Locker::Locker() {
    if (pthread_mutex_init(&mutex_, nullptr) != 0) {
        throw std::exception();
    }
}

Locker::~Locker() {
    pthread_mutex_destroy(&mutex_);
}

auto Locker::Lock() -> bool {
    return pthread_mutex_lock(&mutex_) == 0;
}

auto Locker::Unlock() -> bool {
    return pthread_mutex_unlock(&mutex_) == 0;
}

auto Locker::get() -> pthread_mutex_t * {
    return &mutex_;
}