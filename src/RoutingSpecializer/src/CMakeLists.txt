add_library(routing_specializer SHARED RoutingSpecializer.cpp)
target_link_libraries(routing_specializer PUBLIC traffic_specialization traffic_specializer logger)
target_include_directories(routing_specializer PUBLIC ../include/)
install(TARGETS routing_specializer DESTINATION ${PROJECT_BINARY_DIR}/output/lib)