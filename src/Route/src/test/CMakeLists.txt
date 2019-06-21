add_executable(route-test main.cpp)
target_link_libraries(route-test PUBLIC route)
install(TARGETS route-test RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/output/bin)