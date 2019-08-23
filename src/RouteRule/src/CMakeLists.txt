add_library(route_rule SHARED RouteRule.cpp)
target_link_libraries(route_rule PUBLIC system)
target_include_directories(route_rule PUBLIC ../include/)

install(TARGETS route_rule DESTINATION ${PROJECT_BINARY_DIR}/output/lib)