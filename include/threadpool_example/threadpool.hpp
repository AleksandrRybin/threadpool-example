/*!
 * @file threadpool.hpp
 * @author  (Aleksander Rybin)
 * @brief Threadpool example implementation
 * @date 2021-09-15
 */

#ifndef THREADPOOL_EXAMPLE_THREADPOOL_HPP
#define THREADPOOL_EXAMPLE_THREADPOOL_HPP

#pragma once

// ---------------------
// includes: STL
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <stdexcept>

namespace threadpool_example {

class ThreadPool {
   public:
    ThreadPool(std::size_t = std::thread::hardware_concurrency() - 1);
    ~ThreadPool();

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

   private:
    std::vector<std::thread>          workers;
    std::queue<std::function<void()>> tasks;

    std::mutex              queueMutex;
    std::condition_variable queueCondition;
    bool                    isStop = false;
};

inline ThreadPool::ThreadPool(std::size_t numThreads) {
    for (std::size_t i = 0; i < numThreads; ++i)
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->queueCondition.wait(lock, [this] { return this->isStop || !this->tasks.empty(); });

                    if (this->isStop && this->tasks.empty()) {
                        return;
                    }

                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                task();
            }
        });
}

template <class F, class... Args>
inline auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
    using ReturnType = typename std::result_of<F(Args...)>::type;

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (isStop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
    }

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<ReturnType> resultFuture = task->get_future();
    {
        std::lock_guard<std::mutex> lock(queueMutex);

        tasks.emplace([task] { (*task)(); });
    }
    queueCondition.notify_one();

    return resultFuture;
}

inline ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        isStop = true;
    }

    queueCondition.notify_all();
    for (auto& worker : workers) {
        worker.join();
    }
}

} // namespace threadpool_example

#endif // THREADPOOL_EXAMPLE_THREADPOOL_HPP
