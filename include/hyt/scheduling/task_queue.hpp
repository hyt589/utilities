#pragma once

#include "hyt/defines.hpp"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace hyt {
namespace scheduling {
class DLL_API ITask {
public:
  virtual void Execute() = 0;
};

class DLL_API TaskQueue {
public:
  TaskQueue(const TaskQueue &) = delete;
  TaskQueue(TaskQueue &&) = delete;
  TaskQueue &operator=(const TaskQueue &) = delete;
  TaskQueue &operator=(TaskQueue &&) = delete;

  explicit TaskQueue(const uint32_t max_size);
  ~TaskQueue();
  bool PushTask(std::shared_ptr<ITask> task);
  std::shared_ptr<ITask> PopTask();
  void Stop();
  bool IsStopped();
  bool IsEmpty();

private:
  std::mutex m_queue_mtx;
  std::condition_variable m_cv_pop;
  std::queue<std::shared_ptr<ITask>> m_task_queue;
  uint32_t m_max_size;
  std::atomic_bool m_stopped;
};
} // namespace scheduling

} // namespace hyt
