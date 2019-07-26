add_library(package_rule SHARED ChainRule.cpp PackageRule.cpp)
target_include_directories(package_rule PUBLIC ../include/)


install(TARGETS package_rule DESTINATION ${PROJECT_BINARY_DIR}/output/lib/)