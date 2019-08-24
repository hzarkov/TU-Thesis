add_library(traffic_specialization SHARED TrafficSpecialization.cpp)
target_link_libraries(traffic_specialization PUBLIC network_factory)
target_include_directories(traffic_specialization PUBLIC ../include/)
install(TARGETS traffic_specialization DESTINATION ${PROJECT_BINARY_DIR}/output/lib)