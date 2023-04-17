#include "thread_pool.h"
#include "http_conn.h"

#include <cstdio>
#include <exception>
#include <pthread.h>

template class ThreadPool<HttpConn>;

template <typename T>
ThreadPool<T>::ThreadPool(int thread_num, int max_request) : thread_num_(thread_num), max_requests_(max_request), stop_(false), threads_(nullptr) {
    if ((thread_num <= 0) || (max_request) <= 0) {
        throw std::exception();
    }
    threads_ = new pthread_t[thread_num_];
    if (!threads_) {
        throw std::exception();
    }

    // 创建thread_num_个线程，并将他们设置为线程脱离
    for (int i = 0; i < thread_num_; ++i) {
        printf("create the %dth thread\n", i);

        if (pthread_create(threads_ + i, nullptr, Worker, this) != 0) {
            delete[] threads_;
            throw std::exception();
        }

        if (pthread_detach(threads_[i])) {
            delete[] threads_;
            throw std::exception();
        }
    }
}

template <typename T>
ThreadPool<T>::~ThreadPool() {
    delete[] threads_;
    stop_ = true; // 停止状态
}

template <typename T>
bool ThreadPool<T>::append(T *request) {
    // 操作工作队列时一定要加锁，因为它被所有线程共享
    queue_locker_.lock();
    if (work_queue_.size() > max_requests_) {
        queue_locker_.unlock();
        return false;
    }

    work_queue_.push_back(request);
    queue_locker_.unlock();
    queue_sta_.post();
    return true;
}

template <typename T>
void *ThreadPool<T>::Worker(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    pool->Run();
    return pool;
}

template <typename T>
void ThreadPool<T>::Run() {
    while (!stop_) {
        queue_sta_.wait(); // 没有阻塞就说明有数据
        queue_locker_.lock();
        if (work_queue_.empty()) {
            queue_locker_.unlock();
            continue;
        }
        T *request = work_queue_.front();
        work_queue_.pop_front();
        queue_locker_.unlock();

        if (!request) {
            continue;
        }

        request->Process(); // 任务类
    }
}
