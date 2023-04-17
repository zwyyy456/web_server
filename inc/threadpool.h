#pragma once

#include <pthread.h>
#include <list>
#include <exception>
#include <cstdio>
#include "locker.h"
#include "sem.h"
#include "cond.h"

// 模板类
template <typename T>
class Threadpool {
  public:
    explicit Threadpool(int thead_num = 8, int max_requests = 10000);
    ~Threadpool();

    bool append(T *request);

  private:
    static auto worker(void *arg) -> void *;
    void Run();
    // 线程的数量
    int thread_num_;

    // 线程池数组，大小为thread_num_
    pthread_t *threads_;

    // 请求队列中最多允许的等待处理的请求数量
    int max_requests_;

    // 请求队列
    std::list<T *> work_queue_;

    // 互斥锁
    Locker queue_locker_;

    // 信号量，判断是否有任务需要处理
    Sem queue_stat_;

    // 是否结束线程
    bool stop_;
};

template <typename T>
Threadpool<T>::Threadpool(int thread_num, int max_requests) :
    thread_num_(thread_num), max_requests_(max_requests), stop_(false), threads_(nullptr) {
    if ((thread_num_ <= 0 || (max_requests_ <= 0))) {
        throw std::exception();
    }

    threads_ = new pthread_t[thread_num_];
    if (!threads_) {
        throw std::exception();
    }

    // 创建thread_num_个线程，并设置为detatch
    for (int i = 0; i < thread_num_; ++i) {
        printf("create the %dth thread\n", i + 1);
        if (pthread_create(threads_ + i, nullptr, worker, nullptr, this) != 0) { // worker必须是静态的
            delete[] threads_;
            throw std::exception();
        }
        if (pthread_detach(threads_[i]) != 0) {
            delete[] threads_;
            throw std::exception();
        }
    }
}

template <typename T>
Threadpool<T>::~Threadpool() {
    delete[] threads_;
    stop_ = true; // 说明线程要停止
}

template <typename T>
auto Threadpool<T>::append(T *request) -> bool {
    queue_locker_.Lock();
    if (work_queue_.size() >= max_requests_) { // 这里是大于还是大于等于
        queue_locker_.Unlock();
        return false;
    }
    work_queue_.push_back(request);
    queue_locker_.Unlock();

    queue_stat_.Post(); // 信号量增加
    return true;
}

template <typename T>
auto Threadpool<T>::worker(void *arg) -> void * {
    // 静态成员函数不能访问非静态成员变量，通过在创建线程的时候，使用this指针来解决
    auto pool = static_cast<Threadpool<T> *>(arg);
    pool->Run();
    return nullptr;
}

template <typename T>
auto Threadpool<T>::Run() -> void {
    while (!stop_) {
        queue_stat_.Wait();

        queue_locker_.Lock();
        if (work_queue_.empty()) {
            queue_locker_.Unlock();
            continue;
        }
        T *request = work_queue_.front();
        work_queue_.pop_front();

        queue_locker_.Unlock();
        if (!request) {
            // 没获取到request
        }
        request->Process();
    }
}