#include "cond.h"

Cond::Cond() {
    if (pthread_cond_init(&cond_, nullptr) == 0) {
        throw std::exception();
    }
}

Cond::~Cond() {
    pthread_cond_destroy(&cond_);
}

auto Cond::Wait(pthread_mutex_t *mutex) -> bool {
    return pthread_cond_wait(&cond_, mutex) == 0;
}

auto Cond::TimedWait(pthread_mutex_t *mutex, struct timespec t) -> bool {
    return pthread_cond_timedwait(&cond_, mutex, &t);
}

auto Cond::Signal() -> bool {
    return pthread_cond_signal(&cond_) == 0;
}

auto Cond::Broadcast() -> bool {
    return pthread_cond_broadcast(&cond_) == 0;
}