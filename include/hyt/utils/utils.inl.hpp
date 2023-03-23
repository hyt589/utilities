#pragma once

#include <exception>
#include <functional>
#include <stdexcept>
#include <string>

namespace hyt {

namespace util {

static inline void Assert(const bool condition, const std::string &msg) {
  if (!condition) {
    throw std::domain_error(msg);
  }
}

static inline void
Rethrow(std::function<void(const std::exception &)> callback) {
  auto eptr = std::current_exception();
  if (eptr == nullptr) {
    return;
  }
  try {
    std::rethrow_exception(eptr);
  } catch (const std::exception &e) {
    callback(e);
  }
}

template <typename T> bool InRange(const T lower, const T upper, const T val) {
  static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
  Assert(lower <= upper, "Invalid range");
  return (val >= lower) && (val <= upper);
}
} // namespace util
} // namespace hyt
