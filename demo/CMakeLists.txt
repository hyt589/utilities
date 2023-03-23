set(TGT_NAME demo)

add_executable(demo main.cpp)

target_link_libraries(demo PRIVATE hyt)

if(WIN32)
    add_custom_command(
        TARGET ${TGT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${TGT_NAME}> $<TARGET_FILE_DIR:${TGT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif()
