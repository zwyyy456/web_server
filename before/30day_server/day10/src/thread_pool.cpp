#include "../inc/thread_pool.h"
#include <mutex>
#include <stdexcept>

ThreadPool::ThreadPool(int size) :
    stop_(false) {
    for (int i = 0; i < size; ++i) {
        threads_.emplace_back(std::thread([this]() {
            while (true) {
                std::function<void()> task;
                { // 大括号用于创建 std::unique_lock 对象的作用域，以便锁定 tasks_mtx 互斥量。这确保在作用域退出时释放锁
                    std::unique_lock<std::mutex> lock(tasks_mtx_);
                    conva_.wait(lock, [this]() {
                        return stop_ || !tasks_.empty();
                    });
                    if (stop_ && tasks_.empty()) {
                        return;
                    }
                    task = tasks_.front();
                    tasks_.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(tasks_mtx_);
        stop_ = true;
    }
    conva_.notify_all();
    for (auto &th : threads_) {
        if (th.joinable()) {
            th.join();
        }
    }
}

void ThreadPool::Add(std::function<void()> func) {
    {
        std::unique_lock<std::mutex> lock(tasks_mtx_);
        if (stop_) {
            throw std::runtime_error("ThreadPool already stop, can't add task any more");
        }
        tasks_.emplace(func);
    }
    conva_.notify_one();
}