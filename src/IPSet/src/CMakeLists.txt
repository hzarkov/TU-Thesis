add_library(ipset SHARED IPSet.cpp)

target_link_libraries(ipset PUBLIC system)

target_include_directories(ipset PUBLIC ../include/)

install(TARGETS ipset DESTINATION ${PROJECT_BINARY_DIR}/output/lib)