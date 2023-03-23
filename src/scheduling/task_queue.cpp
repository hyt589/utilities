#include "hyt/scheduling/task_queue.hpp"
#include <iostream>
#include <memory>
#include <mutex>

namespace hyt {
namespace scheduling {
TaskQueue::TaskQueue(const uint32_t max_size)
    : m_max_size(max_size), m_stopped(false){};

TaskQueue::~TaskQueue() { Stop(); }

bool TaskQueue::PushTask(std::shared_ptr<ITask> task) {
  {
    std::unique_lock<std::mutex> lock(m_queue_mtx);
    if (m_task_queue.size() >= m_max_size) {
      return false;
    }
    m_task_queue.push(task);
  }
  m_cv_pop.notify_one();
  return true;
}

void TaskQueue::Stop() {
  m_stopped.store(true);
  m_cv_pop.notify_all();
}

bool TaskQueue::IsStopped() { return m_stopped.load(); }

bool TaskQueue::IsEmpty() {
  std::unique_lock<std::mutex> lock(m_queue_mtx);
  return m_task_queue.empty();
}

std::shared_ptr<ITask> TaskQueue::PopTask() {
  std::unique_lock<std::mutex> lock(m_queue_mtx);
  m_cv_pop.wait(lock,
                [&]() { return !m_task_queue.empty() || m_stopped.load(); });
  if (m_task_queue.empty()) {
    return nullptr;
  }
  auto task = m_task_queue.front();
  m_task_queue.pop();
  return task;
}

} // namespace scheduling
} // namespace hyt
