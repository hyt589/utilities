#pragma once

#include "hyt/defines.hpp"
#include "hyt/scheduling/task_queue.hpp"
#include <memory>
#include <thread>
#include <vector>

namespace hyt {

namespace scheduling {

class DLL_API ThreadPool {
public:
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;

  ThreadPool(const uint32_t concurrency, const uint32_t queue_size);
  ~ThreadPool();

  bool ScheduleTask(std::shared_ptr<ITask> task);

private:
  void WorkerThread();

private:
  std::vector<std::thread> m_threads;
  TaskQueue m_queue;
};

} // namespace scheduling

} // namespace hyt
