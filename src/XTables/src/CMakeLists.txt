add_library(xtables SHARED Chain.cpp Rule.cpp)
target_link_libraries(xtables PUBLIC system)
target_include_directories(xtables PUBLIC ../include/)


install(TARGETS xtables DESTINATION ${PROJECT_BINARY_DIR}/output/lib/)