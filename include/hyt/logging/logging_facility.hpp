#pragma once

#include "hyt/defines.hpp"
#include "hyt/scheduling/task_queue.hpp"
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

namespace hyt {
namespace logging {

struct Entry {
  std::string message;
  std::string timestamp;
  std::string severity;
  std::string tag;
  std::string filename;
  std::thread::id thread_id;
  int line;
};

Entry CreateLogEntry(const std::string &tag, const std::string &msg,
                     const std::string &severity, const std::string &filename,
                     const int32_t line);

class DLL_API ILogger {
public:
  virtual void Log(const Entry &entry) = 0;
};

class DLL_API Dispatcher {
public:
  Dispatcher(const Dispatcher &) = delete;
  Dispatcher(Dispatcher &&) = delete;
  Dispatcher &operator=(const Dispatcher &) = delete;
  Dispatcher &operator=(Dispatcher &&) = delete;

  virtual ~Dispatcher();

  static Dispatcher &Instance();

  void Register(std::shared_ptr<ILogger> logger);
  void Unregister(std::shared_ptr<ILogger> logger);

  void Stop();

  void Trace(const std::string &tag, const std::string &msg,
             const std::string &filename, const int32_t line);
  void Debug(const std::string &tag, const std::string &msg,
             const std::string &filename, const int32_t line);
  void Info(const std::string &tag, const std::string &msg,
            const std::string &filename, const int32_t line);
  void Warn(const std::string &tag, const std::string &msg,
            const std::string &filename, const int32_t line);
  void Error(const std::string &tag, const std::string &msg,
             const std::string &filename, const int32_t line);
  void Fatal(const std::string &tag, const std::string &msg,
             const std::string &filename, const int32_t line);

  void Log(const Entry &entry);

private:
  Dispatcher();

  void Dispatch(const Entry &entry);
  void WorkerThread();

private:
  std::unordered_set<std::shared_ptr<ILogger>> m_logger_set;
  std::thread m_worker_thread;
  scheduling::TaskQueue m_task_queue;
  std::mutex m_mtx;
};

} // namespace logging
} // namespace hyt
