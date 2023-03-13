#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "locker.h"
#include <exception>
#include <list>
#include <pthread.h>

// 线程池，定义成模板类，为了代码的复用
template <typename T>
class ThreadPool {
  public:
    ThreadPool(int thread_num_ = 8, int max_requests_ = 10000);
    ~ThreadPool();

    bool append(T *request);

  private:
    static void *Worker(void *arg);
    void Run();

  private:
    // 线程的数量
    int thread_num_;
    //  线程池数组，大小为thread_num
    pthread_t threads_;

    // 请求队列中最多允许的等待处理的请求数量
    int max_requests_;

    // 请求队列
    std::List<T *> work_queue_;

    // 互斥锁
    Locker queue_locker_;

    // 信号量是用来判断是否有任务需要处理
    Sem queue_sta_;

    // 是否结束
    bool stop_;
};

#endif