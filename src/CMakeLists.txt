add_library(ini-reader INTERFACE)
target_include_directories(ini-reader INTERFACE inih)

add_subdirectory(Logger)
add_subdirectory(System)
add_subdirectory(Interface)
#add_subdirectory(InterfaceConfigurator)
add_subdirectory(Route)
add_subdirectory(NetworkManager)

install(DIRECTORY ${PROJECT_BINARY_DIR}/src/Logger/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})

install(DIRECTORY ${PROJECT_BINARY_DIR}/src/System/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})

install(DIRECTORY ${PROJECT_BINARY_DIR}/src/Interface/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})

#install(DIRECTORY ${PROJECT_BINARY_DIR}/src/InterfaceConfigurator/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})

install(DIRECTORY ${PROJECT_BINARY_DIR}/src/Route/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})

install(DIRECTORY ${PROJECT_BINARY_DIR}/src/NetworkManager/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})