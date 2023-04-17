#include "sem.h"
#include <exception>
#include <semaphore.h>

Sem::Sem() {
    if (sem_init(&sem_, 0, 0) == 0) {
        throw std::exception();
    }
}

Sem::Sem(int num) {
    if (sem_init(&sem_, 0, num) == 0) {
        throw std::exception();
    }
}

Sem::~Sem() {
    sem_destroy(&sem_);
}

auto Sem::Wait() -> bool {
    return sem_wait(&sem_) == 0;
}

auto Sem::Post() -> bool {
    return sem_post(&sem_) == 0;
}