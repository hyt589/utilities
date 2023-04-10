#ifndef HYT_THREAD_POOL
#define HYT_THREAD_POOL

#include "hyt/utils/lock_free_queue.tpl.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

namespace hyt {

namespace concurrency {

template <typename T> using Queue = lockfree::Queue<T>;

class ThreadPool {
public:
  explicit ThreadPool(
      const size_t concurrency = std::thread::hardware_concurrency() - 1,
      const size_t queue_size = 1 << 4)
      : m_queue(queue_size), m_concurrency(concurrency) {
    Restart();
  }

  ~ThreadPool() {
    if (!IsStopped()) {
      Stop();
    }
  }

  template <class F, class... Args>
  auto Async(F &&f, Args &&...args) -> std::future<decltype(f(args...))> {
    using ReturnType = decltype(f(args...));
    using Task = std::function<ReturnType()>;
    Task task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto pkg_task = std::make_shared<std::packaged_task<ReturnType()>>(task);
    m_queue.Push([pkg_task]() { (*pkg_task)(); });
    return pkg_task->get_future();
  }

  void Stop(const bool wait_for_tasks = true) {
    while (m_queue.Size() != 0 && wait_for_tasks) {
      std::this_thread::yield();
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    m_stopped.store(true);
    for (auto &thread : m_workers) {
      if (thread.joinable()) {
        thread.join();
      }
    }
    m_workers.clear();
  }

  void Restart() {
    m_stopped.store(false);
    for (size_t i = 0; i < m_concurrency; i++) {
      m_workers.emplace_back(std::thread([this]() {
        std::function<void()> task;
        while (!m_stopped.load()) {
          if (m_queue.Pop(task)) {
            task();
          }
        }
      }));
    }
  }

  bool IsStopped() { return m_stopped.load(); }

private:
  Queue<std::function<void()>> m_queue;
  std::vector<std::thread> m_workers;
  std::atomic_bool m_stopped{false};
  const size_t m_concurrency;
};

} // namespace concurrency
} // namespace hyt

#endif
