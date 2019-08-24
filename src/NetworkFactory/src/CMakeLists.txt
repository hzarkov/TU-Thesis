add_library(network_factory SHARED NetworkFactory.cpp)
target_link_libraries(network_factory PUBLIC ethernet_interface_controller route logger stdc++fs ini_reader dnsmasq_controller xtables ipset route_rule)
target_include_directories(network_factory PUBLIC ../include/)
target_compile_options(network_factory PRIVATE -fPIC)

install(TARGETS network_factory DESTINATION ${PROJECT_BINARY_DIR}/output/lib/)