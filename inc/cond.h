#pragma once
#include <pthread.h>
#include <exception>
#include "locker.h"

class Cond {
  public:
    Cond();
    ~Cond();

    auto Wait(pthread_mutex_t *mutex) -> bool;
    auto TimedWait(pthread_mutex_t *mutex, struct timespec t) -> bool;
    auto Signal() -> bool;
    auto Broadcast() -> bool;

  private:
    pthread_cond_t cond_;
};