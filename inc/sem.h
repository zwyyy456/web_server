#pragma once
#include <pthread.h>
#include <exception>
#include <semaphore.h>
#include "locker.h"

class Sem {
  public:
    Sem();
    explicit Sem(int num);
    ~Sem();

    auto Wait() -> bool;
    auto Post() -> bool;

  private:
    sem_t sem_;
};
