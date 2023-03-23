#include "hyt/logging/cout_logger.hpp"
#include "hyt/logging/logging_facility.hpp"
#include "hyt/logging/macros.hpp"
#include "hyt/utils/utils.inl.hpp"
#include <exception>
#include <iostream>
#include <memory>

int main() {
  try {
    auto logger = std::make_shared<hyt::logging::CoutLogger>();
    hyt::logging::Dispatcher::Instance().Register(logger);
    LOG_D(main) << "hello " << 1 << " " << false;
    LOG_D(main) << "hello " << 2 << " " << false;
    LOG_D(main) << "hello " << 3 << " " << false;
    LOG_D(main) << "hello " << 4 << " " << false;
    LOG_D(main) << "hello " << 5 << " " << false;
    hyt::logging::Dispatcher::Instance().Stop();
  } catch (...) {
    hyt::util::Rethrow(
        [](const std::exception e) { std::cout << e.what() << std::endl; });
  }
}
