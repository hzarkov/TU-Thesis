add_library(xtables SHARED Table.cpp Chain.cpp ChainRule.cpp)
target_link_libraries(xtables PUBLIC system logger)
target_include_directories(xtables PUBLIC ../include/)


install(TARGETS xtables DESTINATION ${PROJECT_BINARY_DIR}/output/lib/)