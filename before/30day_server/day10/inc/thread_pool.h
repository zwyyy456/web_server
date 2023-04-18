#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool {
  private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_; // 任务队列
    std::mutex tasks_mtx_;
    std::condition_variable conva_;
    bool stop_;

  public:
    ThreadPool(int size = 10);
    ~ThreadPool();

    void Add(std::function<void()>);
};