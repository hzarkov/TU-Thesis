add_library(logger SHARED Logger.cpp)
target_include_directories(logger PUBLIC ../include/)
install(TARGETS logger DESTINATION ${PROJECT_BINARY_DIR}/output/lib)

add_subdirectory(tests)