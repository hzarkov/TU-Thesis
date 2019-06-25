add_library(dnsmasq_controller SHARED DNSMasqController.cpp)
target_link_libraries(dnsmasq_controller PUBLIC system)
target_include_directories(dnsmasq_controller PUBLIC ../include/)
install(TARGETS dnsmasq_controller DESTINATION ${PROJECT_BINARY_DIR}/output/lib)