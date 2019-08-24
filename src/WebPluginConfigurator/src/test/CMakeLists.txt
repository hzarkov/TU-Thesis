add_executable(test_server server.cpp)
target_link_libraries(test_server PRIVATE web_plugin_configurator)
#target_include_directories(test_server PRIVATE ../include/)

install(TARGETS test_server DESTINATION ${PROJECT_BINARY_DIR}/output/bin/)