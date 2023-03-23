#include "hyt/scheduling/thread_pool.hpp"

namespace hyt {
namespace scheduling {

ThreadPool::ThreadPool(const uint32_t concurrency, const uint32_t queue_size)
    : m_queue(queue_size) {
  for (int i = 0; i < concurrency; i++) {
    m_threads.emplace_back(std::thread([&]() { WorkerThread(); }));
  }
}

ThreadPool::~ThreadPool() {
  m_queue.Stop();
  for (auto &thread : m_threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

bool ThreadPool::ScheduleTask(std::shared_ptr<ITask> task) {
  return m_queue.PushTask(task);
}

void ThreadPool::WorkerThread() {
  while (!m_queue.IsStopped()) {
    auto task = m_queue.PopTask();
    if (!task) {
      return;
    }
    task->Execute();
  }
}

} // namespace scheduling

} // namespace hyt
