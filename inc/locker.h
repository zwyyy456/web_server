#pragma once

#include <pthread.h>
#include <exception>

// 线程同步机制封装类
// 控制线程对任务队列的同步访问
// 互斥锁类

class Locker {
  public:
    Locker();
    ~Locker();

    auto Lock() -> bool;
    auto Unlock() -> bool;
    auto get() -> pthread_mutex_t *;

  private:
    pthread_mutex_t mutex_;
};