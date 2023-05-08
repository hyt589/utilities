#pragma once

#include "hyt/concurrency/lockfree/queue.tpl.hpp"
#include "hyt/defines.hpp"
#include "hyt/scheduling/task_queue.hpp"
#include <atomic>
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
  ILogger();
  ~ILogger();
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

  void Register(ILogger *logger);
  void Unregister(ILogger *logger);

  void Start();
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
  using MessageQueue = hyt::concurrency::lockfree::Queue<Entry>;

  std::unordered_set<ILogger *> m_logger_set;
  std::thread m_worker_thread;
  /* scheduling::TaskQueue m_task_queue; */
  MessageQueue m_msg_queue;
  std::mutex m_mtx;
  std::atomic_bool m_running{false};
};

} // namespace logging
} // namespace hyt
