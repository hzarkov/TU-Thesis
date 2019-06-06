add_subdirectory(Logger)
add_subdirectory(System)
add_subdirectory(Interface)
add_subdirectory(InterfaceConfigurator)


install(DIRECTORY ${PROJECT_BINARY_DIR}/src/Logger/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})

install(DIRECTORY ${PROJECT_BINARY_DIR}/src/System/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})

install(DIRECTORY ${PROJECT_BINARY_DIR}/src/Interface/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})

install(DIRECTORY ${PROJECT_BINARY_DIR}/src/InterfaceConfigurator/output/ USE_SOURCE_PERMISSIONS DESTINATION ${FLASH_DIR})