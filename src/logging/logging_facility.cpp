#include "hyt/logging/logging_facility.hpp"
#include "hyt/scheduling/task_queue.hpp"
#include "hyt/utils/utils.inl.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

namespace hyt {
namespace logging {

class LoggingTask : public scheduling::ITask {
public:
  LoggingTask(const Entry &entry, std::shared_ptr<ILogger> logger)
      : m_entry(entry), m_logger_ptr(logger) {}

  virtual void Execute() override { m_logger_ptr->Log(m_entry); }

private:
  Entry m_entry;
  std::shared_ptr<ILogger> m_logger_ptr;
};

std::string TimestampNow() {
  auto now = std::chrono::system_clock::now();
  auto time_now = std::chrono::system_clock::to_time_t(now);

  char date[64];
  struct tm *time;
  time = std::localtime(&time_now); // NOLINT
  strftime(date, 64, "%Y-%m-%d %H:%M:%S", time);
  return date;
}

Entry CreateLogEntry(const std::string &tag, const std::string &msg,
                     const std::string &severity, const std::string &filename,
                     const int32_t line) {
  Entry message;
  message.tag = tag;
  message.message = msg;
  message.severity = severity;
  message.timestamp = TimestampNow();
  message.filename = filename;
  message.line = line;
  message.thread_id = std::this_thread::get_id();
  return message;
}

Dispatcher::Dispatcher()
    : m_worker_thread(std::thread([&]() { WorkerThread(); })),
      m_task_queue(500) {}

Dispatcher::~Dispatcher() { Stop(); }

void Dispatcher::Stop() {
  int attempt = 0;
  while (!m_task_queue.IsEmpty()) {
    if (attempt >= 5) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    attempt++;
  }
  m_task_queue.Stop();
  if (m_worker_thread.joinable()) {
    m_worker_thread.join();
  }
}

void Dispatcher::Register(std::shared_ptr<ILogger> logger) {
  m_logger_set.insert(logger);
}

void Dispatcher::Unregister(std::shared_ptr<ILogger> logger) {
  m_logger_set.erase(logger);
}

void Dispatcher::Trace(const std::string &tag, const std::string &msg,
                       const std::string &filename, const int32_t line) {
  Dispatch(CreateLogEntry(tag, msg, "Trace", filename, line));
}
void Dispatcher::Debug(const std::string &tag, const std::string &msg,
                       const std::string &filename, const int32_t line) {
  Dispatch(CreateLogEntry(tag, msg, "Debug", filename, line));
}
void Dispatcher::Info(const std::string &tag, const std::string &msg,
                      const std::string &filename, const int32_t line) {
  Dispatch(CreateLogEntry(tag, msg, "Info", filename, line));
}
void Dispatcher::Warn(const std::string &tag, const std::string &msg,
                      const std::string &filename, const int32_t line) {
  Dispatch(CreateLogEntry(tag, msg, "Warn", filename, line));
}
void Dispatcher::Error(const std::string &tag, const std::string &msg,
                       const std::string &filename, const int32_t line) {
  Dispatch(CreateLogEntry(tag, msg, "Error", filename, line));
}
void Dispatcher::Fatal(const std::string &tag, const std::string &msg,
                       const std::string &filename, const int32_t line) {
  Dispatch(CreateLogEntry(tag, msg, "Fatal", filename, line));
}

void Dispatcher::Log(const Entry &entry) { Dispatch(entry); }

void Dispatcher::Dispatch(const Entry &entry) {
  for (auto logger : m_logger_set) {
    auto task = std::make_shared<LoggingTask>(entry, logger);
    while (!m_task_queue.PushTask(task)) {
      // Try again after 5 ms until success
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }
}

void Dispatcher::WorkerThread() {
  while (!m_task_queue.IsStopped() || !m_task_queue.IsEmpty()) {
    auto task = m_task_queue.PopTask();
    if (!task) {
      continue;
    }
    try {
      task->Execute();
    } catch (...) {
      util::Rethrow(
          [](const std::exception &e) { std::cout << e.what() << std::endl; });
    }
  }
}

Dispatcher &Dispatcher::Instance() {
  static Dispatcher kInstance;
  return kInstance;
}

} // namespace logging
} // namespace hyt
