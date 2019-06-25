add_library(dhcp_server SHARED DHCPServer.cpp DNSMasqDHCPServerConfiguration.cpp)
target_link_libraries(dhcp_server PUBLIC dnsmasq_controller)
target_include_directories(dhcp_server PUBLIC ../include/)
install(TARGETS dhcp_server DESTINATION ${PROJECT_BINARY_DIR}/output/lib)