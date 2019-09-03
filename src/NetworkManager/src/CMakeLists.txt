add_executable(network-manager main.cpp)
target_link_libraries(network-manager PRIVATE ini_reader network_factory dloader nm_plugin web_plugin_configurator)
#target_include_directories(network-manager PRIVATE ../include/)

install(TARGETS network-manager DESTINATION ${PROJECT_BINARY_DIR}/output/bin/)
install(FILES nmPluginConfiguration.ini DESTINATION ${PROJECT_BINARY_DIR}/output/config/)