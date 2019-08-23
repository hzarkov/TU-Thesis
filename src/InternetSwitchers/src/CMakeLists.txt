add_library(ping_internet_switcher SHARED PingInternetSwitcher.cpp)
target_link_libraries(ping_internet_switcher PUBLIC system nm_plugin logger network_controller)
target_include_directories(ping_internet_switcher PUBLIC ../include/)

install(TARGETS ping_internet_switcher DESTINATION ${PROJECT_BINARY_DIR}/output/lib/)