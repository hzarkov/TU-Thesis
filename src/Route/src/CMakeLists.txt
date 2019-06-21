add_library(route SHARED Route.cpp)
target_link_libraries(route PUBLIC system interface logger)
target_include_directories(route PUBLIC ../include/)
install(TARGETS route DESTINATION ${PROJECT_BINARY_DIR}/output/lib)

add_subdirectory(test)