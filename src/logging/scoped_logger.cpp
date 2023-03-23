#include "hyt/logging/scoped_logger.hpp"
#include "hyt/logging/logging_facility.hpp"
#include <iostream>
#include <stdint.h>
#include <string>

namespace hyt {

namespace logging {
ScopedLogger::ScopedLogger(const std::string &filename, const int32_t line,
                           const std::string &tag, const std::string &severity)
    : m_filename(filename), m_severity(severity), m_line(line), m_tag(tag) {}

ScopedLogger::~ScopedLogger() {
  auto msg = m_msg_stream.str();
  Dispatcher::Instance().Log(
      CreateLogEntry(m_tag, msg, m_severity, m_filename, m_line));
};

} // namespace logging

} // namespace hyt
