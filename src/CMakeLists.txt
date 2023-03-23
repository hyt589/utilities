if(HYT_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED)
else()
    add_library(${PROJECT_NAME} STATIC)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>)

target_sources(${PROJECT_NAME} PRIVATE
    ./logging/logging_facility.cpp
    ./logging/scoped_logger.cpp
    ./scheduling/task_queue.cpp
    ./scheduling/thread_pool.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE HYT_EXPORT)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
