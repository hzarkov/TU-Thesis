add_library(system SHARED System.cpp)
target_include_directories(system PUBLIC ../include/)
install(TARGETS system DESTINATION ${PROJECT_BINARY_DIR}/output/lib)