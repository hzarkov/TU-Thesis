add_library(network_controller SHARED NetworkManager.cpp)
target_link_libraries(network_controller PUBLIC ethernet_interface_controller route logger stdc++fs ini_reader dnsmasq_controller xtables)
target_include_directories(network_controller PUBLIC ../include/)
target_compile_options(network_controller PRIVATE -fPIC)

add_library(nm_plugin SHARED Plugin.cpp)
target_link_libraries(nm_plugin PUBLIC network_controller )
target_include_directories(nm_plugin PUBLIC ../include/)

add_library(nm_configurator SHARED Configurator.cpp)
target_link_libraries(nm_configurator PUBLIC nm_plugin )
target_include_directories(nm_configurator PUBLIC ../include/)

add_library(internet_switcher SHARED InternetSwitcher.cpp)
target_link_libraries(internet_switcher PUBLIC nm_plugin )
target_include_directories(internet_switcher PUBLIC ../include/)

add_executable(network-manager main.cpp)
target_link_libraries(network-manager PRIVATE ini_reader network_controller ini_nm_configurator dloader nm_configurator)
target_include_directories(network-manager PRIVATE ../include/)

install(TARGETS nm_plugin internet_switcher nm_configurator network_controller DESTINATION ${PROJECT_BINARY_DIR}/output/lib/)
install(TARGETS network-manager DESTINATION ${PROJECT_BINARY_DIR}/output/bin/)
install(FILES nmPluginConfiguration.ini configuration.ini DESTINATION ${PROJECT_BINARY_DIR}/output/config/)