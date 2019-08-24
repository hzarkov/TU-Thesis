add_library(plugin_configurator SHARED PluginConfigurator.cpp)
target_link_libraries(plugin_configurator PUBLIC nm_plugin)
target_include_directories(plugin_configurator PUBLIC ../include/)
install(TARGETS plugin_configurator DESTINATION ${PROJECT_BINARY_DIR}/output/lib)