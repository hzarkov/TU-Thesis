set(DHCPCLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dhcpclient-snap_20071104/)
add_custom_command(
   OUTPUT ${DHCPCLIENT_DIR}
   COMMAND make
   WORKING_DIRECTORY ${DHCPCLIENT_DIR}
   )

add_custom_target(
   dhcpclient_binary
   DEPENDS ${DHCPCLIENT_DIR}
)
install(FILES ${DHCPCLIENT_DIR}/dhcpclient DESTINATION ${PROJECT_BINARY_DIR}/output/bin/
    PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE WORLD_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

add_library(dhcpclient DHCPClient.cpp)
target_link_libraries(dhcpclient PRIVATE system)
target_include_directories(dhcpclient PUBLIC ../include/)
install(TARGETS dhcpclient DESTINATION ${PROJECT_BINARY_DIR}/output/lib/)