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

inline auto GetFileDir(const char *file_macro) {
  std::string file_path(file_macro);
#ifdef _WIN32
  char delim = '\\';
#else
  char delim = '/';
#endif
  return file_path.substr(0, file_path.rfind(delim) + 1);
}

#define FILE_DIR ::hyt::util::GetFileDir(__FILE__)

} // namespace util
} // namespace hyt
