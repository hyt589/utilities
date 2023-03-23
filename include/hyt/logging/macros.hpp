#pragma once

#include "hyt/logging/scoped_logger.hpp"

#define LOG(tag, level)                                                        \
  if (true)                                                                    \
  hyt::logging::ScopedLogger(__FILE__, __LINE__, tag, level)

#define LOG_T(tag) LOG(#tag, "TRACE")
#define LOG_D(tag) LOG(#tag, "DEBUG")
#define LOG_I(tag) LOG(#tag, "INFO")
#define LOG_W(tag) LOG(#tag, "WARN")
#define LOG_E(tag) LOG(#tag, "ERROR")
#define LOG_F(tag) LOG(#tag, "FATAL")
