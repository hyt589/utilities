#include "hyt/logging/logging_facility.hpp"
#include "hyt/scheduling/task_queue.hpp"
#include "hyt/utils/utils.inl.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>

namespace hyt {
namespace logging {

class LoggingTask : public scheduling::ITask {
public:
  LoggingTask(const Entry &entry, ILogger *logger)
      : m_entry(entry), m_logger_ptr(logger) {}

  virtual void Execute() override { m_logger_ptr->Log(m_entry); }

private:
  Entry m_entry;
  ILogger *m_logger_ptr;
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

Dispatcher::Dispatcher() : m_msg_queue(500) { Start(); }

Dispatcher::~Dispatcher() { Stop(); }

void Dispatcher::Start() {
  if (m_running.load()) {
    return;
  }
  m_running.store(true);
  m_worker_thread = std::thread([this]() { WorkerThread(); });
}

void Dispatcher::Stop() {
  while (m_msg_queue.Size() != 0) {
    std::this_thread::yield();
  }
  m_running.store(false);
  if (m_worker_thread.joinable()) {
    m_worker_thread.join();
  }
}

void Dispatcher::Register(ILogger *logger) { m_logger_set.insert(logger); }

void Dispatcher::Unregister(ILogger *logger) { m_logger_set.erase(logger); }

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
  while (!m_msg_queue.Push(entry)) {
    std::this_thread::yield();
  }
}

void Dispatcher::WorkerThread() {
  while (m_running.load()) {
    Entry entry;
    if (m_msg_queue.Pop(entry)) {
      for (auto logger : m_logger_set) {
        try {
          logger->Log(entry);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
        } catch (...) {
          std::cerr << "Unknown error while handling log message" << std::endl;
        }
      }
    }
    std::this_thread::yield();
  }
}

Dispatcher &Dispatcher::Instance() {
  static Dispatcher kInstance;
  return kInstance;
}

ILogger::ILogger() { Dispatcher::Instance().Register(this); }

ILogger::~ILogger() { Dispatcher::Instance().Unregister(this); }

} // namespace logging
} // namespace hyt
