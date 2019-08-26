add_library(ethernet_interface_controller SHARED EthernetController.cpp InterfaceController.cpp)
target_link_libraries(ethernet_interface_controller PUBLIC interface dhcp_client dhcp_server)
target_include_directories(ethernet_interface_controller PUBLIC ../include/)


install(TARGETS ethernet_interface_controller DESTINATION ${PROJECT_BINARY_DIR}/output/lib/)