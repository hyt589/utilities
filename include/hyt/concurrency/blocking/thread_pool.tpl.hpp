#pragma once

#include "hyt/concurrency/blocking/queue.tpl.hpp"

#include <atomic>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <stdint.h>
#include <thread>
#include <vector>

namespace hyt {

namespace concurrency {

namespace blocking {

class ThreadPool {
public:
  ThreadPool(const uint32_t concurrency = std::thread::hardware_concurrency())
      : m_concurrency(concurrency) {
    Restart();
  }

  virtual ~ThreadPool() { Stop(); }

  void Restart() {
    if (m_running.load()) {
      throw std::exception("Thread pool already running");
    }
    m_workers.clear();
    m_queue.EnableBlock(true);
    m_running = true;
    for (uint32_t i = 0; i < m_concurrency; i++) {
      m_workers.emplace_back(std::thread([this]() {
        while (m_running.load()) {
          std::this_thread::yield();
          std::function<void()> task;
          if (m_queue.WaitAndPop(task)) {
            task();
          }
        }
      }));
    }
  }

  void Stop() {
    m_running = false;
    m_queue.EnableBlock(false);
    m_queue.NotifyAll();
    for (auto &worker : m_workers) {
      if (worker.joinable()) {
        worker.join();
      }
    }
    m_workers.clear();
  }

  bool IsRunning() { return m_running; }

  template <class F, class... Args>
  auto Async(F &&f, Args &&...args) -> std::future<decltype(f(args...))> {
    using ReturnType = decltype(f(args...));
    using Task = std::function<ReturnType()>;
    Task task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto pkg_task = std::make_shared<std::packaged_task<ReturnType()>>(task);
    m_queue.Push([pkg_task]() { (*pkg_task)(); });
    return pkg_task->get_future();
  }

private:
  std::atomic_bool m_running{false};
  const uint32_t m_concurrency;
  std::vector<std::thread> m_workers;
  Queue<std::function<void()>> m_queue;
};

} // namespace blocking

} // namespace concurrency

} // namespace hyt
