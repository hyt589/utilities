#pragma once

#include "hyt/defines.hpp"
#include "hyt/logging/logging_facility.hpp"
#include <sstream>
namespace hyt {

namespace logging {

class DLL_API ScopedLogger {
public:
  ScopedLogger(const std::string &filename, const int32_t line,
               const std::string &tag, const std::string &severity);
  ~ScopedLogger();
  ScopedLogger(const ScopedLogger &) = delete;
  ScopedLogger(ScopedLogger &&) = delete;
  ScopedLogger &operator=(const ScopedLogger &) = delete;
  ScopedLogger &operator=(ScopedLogger &&) = delete;

  template <typename T> ScopedLogger &operator<<(const T &msg) {
    m_msg_stream << msg;
    return *this;
  }

private:
  std::stringstream m_msg_stream;
  std::string m_filename;
  int32_t m_line;
  std::string m_tag;
  std::string m_severity;
};

} // namespace logging

} // namespace hyt
