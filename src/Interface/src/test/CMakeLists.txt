add_executable(interface-test main.cpp)
target_link_libraries(interface-test PUBLIC interface logger)
install(TARGETS interface-test RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/output/bin)