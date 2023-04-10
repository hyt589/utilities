#pragma once

#include "hyt/logging/logging_facility.hpp"
#include <iostream>
#include <sstream>

namespace hyt {

namespace logging {

class CoutLogger : public ILogger {
public:
  virtual void Log(const Entry &entry) override {
    std::stringstream ss;
    ss << "[" << entry.tag << "][" << entry.timestamp << "][" << entry.severity
       << "][" << entry.filename << ":" << entry.line << "]" << entry.message;
    std::cout << ss.str() << std::endl;
  }
};
} // namespace logging

} // namespace hyt
