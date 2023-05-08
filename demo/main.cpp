#include "hyt/concurrency/blocking/thread_pool.tpl.hpp"
#include "hyt/logging/cout_logger.hpp"
#include "hyt/logging/logging_facility.hpp"
#include "hyt/logging/macros.hpp"
#include "hyt/utils/utils.inl.hpp"
#include <exception>
#include <iostream>
#include <memory>

using hyt::concurrency::blocking::ThreadPool;
using hyt::logging::CoutLogger;
using hyt::logging::Dispatcher;
using hyt::util::Rethrow;

int main() {
  try {

    ThreadPool thread_pool(1);

    thread_pool.Async([]() { std::cout << "async" << std::endl; }).wait();

    auto logger = std::make_shared<CoutLogger>();
    LOG_D(main) << "hello " << 1 << " " << false;
    LOG_D(main) << "hello " << 2 << " " << false;
    LOG_D(main) << "hello " << 3 << " " << false;
    LOG_D(main) << "hello " << 4 << " " << false;
    LOG_D(main) << "hello " << 5 << " " << false;
#ifdef _MSC_VER
    // static objects destructors are not called by end of main on msvc
    Dispatcher::Instance().Stop();
#endif

  } catch (...) {
    Rethrow([](const std::exception e) { std::cout << e.what() << std::endl; });
  }
}
