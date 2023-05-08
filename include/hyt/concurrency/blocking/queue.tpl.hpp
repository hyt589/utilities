#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace hyt {

namespace concurrency {

namespace blocking {

template <typename T> class Queue {
public:
  void Push(T const &data) {
    {
      std::lock_guard<std::mutex> lock(m_guard);
      m_queue.push(data);
    }
    m_signal.notify_one();
  }

  bool Empty() const {
    std::lock_guard<std::mutex> lock(m_guard);
    return m_queue.empty();
  }

  /* bool TryPop(T &item) { */
  /*   std::lock_guard<std::mutex> lock(m_guard); */
  /*   if (m_queue.empty()) { */
  /*     return false; */
  /*   } */

  /*   item = m_queue.front(); */
  /*   m_queue.pop(); */
  /*   return true; */
  /* } */

  bool WaitAndPop(T &item) {
    std::unique_lock<std::mutex> lock(m_guard);
    m_signal.wait(lock, [this]() { return !m_blocking || !m_queue.empty(); });
    if (m_queue.empty()) {
      return false;
    }
    item = m_queue.front();
    m_queue.pop();
    return true;
  }

  void NotifyAll() { m_signal.notify_all(); }

  void EnableBlock(const bool blocking) {
    m_blocking = blocking;
    if (!blocking) {
      NotifyAll();
    }
  }

  /* bool TryWaitAndPop(T &item, int time_ms) { */
  /*   std::unique_lock<std::mutex> lock(m_guard); */
  /*   while (m_queue.empty()) { */
  /*     m_signal.wait_for(lock, std::chrono::milliseconds(time_ms)); */
  /*     return false; */
  /*   } */

  /*   item = m_queue.front(); */
  /*   m_queue.pop(); */
  /*   return true; */
  /* } */

private:
  std::queue<T> m_queue;
  mutable std::mutex m_guard;
  std::condition_variable m_signal;
  bool m_blocking = false;
};

} // namespace blocking

} // namespace concurrency

} // namespace hyt
