add_library(ethernet_controller SHARED EthenetController.cpp)
target_include_directories(ethernet_controller PUBLIC ../include/ethernet/)
install(TARGETS ethernet_controller DESTINATION ${PROJECT_BINARY_DIR}/output/lib)