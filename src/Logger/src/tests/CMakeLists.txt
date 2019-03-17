add_executable(logger-test test.cpp)
target_link_libraries(logger-test logger)
install(TARGETS logger-test RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/output/bin)