add_library(nm_plugin SHARED Plugin.cpp)
target_link_libraries(nm_plugin PUBLIC network_factory )
target_include_directories(nm_plugin PUBLIC ../include/)

install(TARGETS nm_plugin DESTINATION ${PROJECT_BINARY_DIR}/output/lib/)