add_library(dnsmasq_configuration SHARED DNSMasqConfiguration.cpp)
#target_link_libraries(dnsmasq_configuration PUBLIC system)
target_include_directories(dnsmasq_configuration PUBLIC ../include/)

install(TARGETS dnsmasq_configuration DESTINATION ${PROJECT_BINARY_DIR}/output/lib)